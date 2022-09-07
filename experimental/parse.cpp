
#include <cassert>
#include <exception>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>


/* THE FRAMEWORK */

template<typename T>
using ParseResult = std::optional<std::pair<T, std::string_view>>;

template<typename T>
struct Parser {
public:
    virtual ParseResult<T> operator()(std::string_view input) const = 0;

    virtual ~Parser() {
    };
};

/* Exceptions give us the funky control flow we need to make the interface feel
 * nice to use. */
struct MonadicParseException : std::runtime_error {
public:
    MonadicParseException(const char* str) : std::runtime_error(str) {}
};

struct ParserContext {
private:
    std::string_view input;

    ParserContext(std::string_view input) : input {input} {}

public:
    template<typename T>
    T bind(const Parser<T>& parser) {
        ParseResult<T> res {parser(input)};
        if (!res.has_value()) {
            throw MonadicParseException("inner parser failed");
        }

        auto [u, input_2] = res.value();
        input = input_2;
        return u;
    }

    void fail() {
        throw MonadicParseException("ParserContext.fail() called");
    }

    bool done() {
        return input.size() == 0;
    }

    template<typename T> friend ParseResult<T> monadic_parse(std::string_view input, std::function<T(ParserContext&)> func);
};

/* I claim that this pattern is monadic. Even so, the parser type here differs a 
 * bit from Haskell's parser monad, so the interface is not exactly the same. 
 * I really had lexers on the mind, so the supported grammars might be on the 
 * simpler side (context free?). A simple backtracking system may be possible,
 * but it likely won't backtrack for all the parsers (only a locally available subset). */
template<typename T>
ParseResult<T> monadic_parse(std::string_view input, std::function<T(ParserContext&)> func) {
    try {
        ParserContext ctx {input};
        return std::pair(func(ctx), ctx.input);
    }
    catch (MonadicParseException e) { 
        return std::nullopt;
    }
}


/* THE LIBRARY */

struct ParseInt : public Parser<int> {
private:
    bool isDigit(char ch) const {
        return (ch >= '0' && ch <= '9');
    }

public:
    ParseResult<int> operator()(std::string_view input) const override {
        // TODO: add negatives, and overflow detection.
        int buffer = 0;
        int consumed_count = 0;
        for (char ch : input) {
            if (isDigit(ch)) {
                buffer *= 10;
                buffer += ch - '0';
                consumed_count++;
            }
            else break;
        }

        if (consumed_count == 0)
            return std::nullopt;
        
        input.remove_prefix(consumed_count);
        return std::pair(buffer, input);
    }
};

struct ParseKnownChar : public Parser<char> {
private:
    char val;

public:
    ParseKnownChar(char val) : val {val} {}

    ParseResult<char> operator()(std::string_view input) const override {
        if (input.size() == 0 || input[0] != val)
            return std::nullopt;

        input.remove_prefix(1);
        return std::pair(val, input);
    }
};

template<typename T>
struct ParseFirstMatch : public Parser<T> {
private:
    std::vector<const Parser<T>*> parsers;

public:
    ParseFirstMatch(std::vector<const Parser<T>*> parsers) : parsers {parsers} {}

    ParseResult<T> operator()(std::string_view input) const override {
        for (auto parser : parsers) {
            ParseResult<T> res {(*parser)(input)};
            if (res.has_value())
                return res;
        }

        return std::nullopt;
    }
};

template<typename T>
struct ParseAttempt : public Parser<std::optional<T>> {
private:
    const Parser<T>& inner;

public:
    ParseAttempt (const Parser<T>& inner) : inner {inner} {}

    ParseResult<std::optional<T>> operator()(std::string_view input) const override {
        ParseResult<T> res {inner(input)};
        if (res.has_value()) {
            return std::pair {res.value().first, res.value().second};
        }

        return std::pair {std::nullopt, input};
    }
};

/* THE DEMO CODE */

struct rgb {
public:
    int r;
    int g;
    int b;
};

struct ParseRGB : public Parser<rgb> {
public:
    ParseResult<rgb> operator()(std::string_view input) const override {
        return monadic_parse<rgb>(input, [](ParserContext& ctx) {
            ctx.bind(ParseKnownChar {'('});
            int r = ctx.bind(ParseInt {});
            ctx.bind(ParseKnownChar {','});
            int g = ctx.bind(ParseInt {});
            ctx.bind(ParseKnownChar {','});
            int b = ctx.bind(ParseInt {});
            ctx.bind(ParseKnownChar {')'});

            if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
                ctx.fail();

            return rgb {r, g, b};
        });
    }
};

/* I feel like almost any context free grammar is possible, even where the terms are recursive. 
 * This combinator models a simple right recursive grammar. A left recursive grammar may 
 * require more effort. (Parser combinators tend to struggle there anyhow.) */
struct ParseParseltongue : public Parser<std::string> {
    // Example parseltongue words: "s", "sss", "sssssss", "";
public:
    ParseResult<std::string> operator()(std::string_view input) const override {
        return monadic_parse<std::string>(input, [](ParserContext& ctx) {
            if (ctx.done())
                return std::string("");

            ctx.bind(ParseKnownChar('s')); // A bind without a variable is more like >> (then) than >>= (bind);

            std::string ans = ctx.bind(ParseParseltongue{});
            ans.push_back('s');
            return ans;
        });
    }
};

/* Lets try math expressions (+ - * / but no whitespace for convenience) */

/* We could certainly be a bit more typesafe. */
struct MultDivExpr {
public:
    char op;
    int left;
    std::shared_ptr<MultDivExpr> right;

    int eval() {
        if (op == ' ')
            return left;
        if (op == '*')
            return left * right->eval();
        if (op == '/')
            return left / right->eval();
        assert(false);
    }
};

struct PlusMinusExpr {
public:
    char op;
    MultDivExpr left;
    std::shared_ptr<PlusMinusExpr> right;

    int eval() {
        if (op == ' ')
            return left.eval();
        if (op == '+')
            return left.eval() + right->eval();
        if (op == '-')
            return left.eval() - right->eval();
        assert(false);
    }
};

struct ParseMultDivExpr : public Parser<MultDivExpr> {
private:
    ParseKnownChar mult {'*'};
    ParseKnownChar div {'/'};
public:
    ParseResult<MultDivExpr> operator()(std::string_view input) const override {
        return monadic_parse<MultDivExpr>(input, [this](ParserContext& ctx) -> MultDivExpr {
            int left {ctx.bind(ParseInt{})};

            std::optional<char> op {ctx.bind(ParseAttempt(ParseFirstMatch<char>({&mult, &div})))};
            if (!op.has_value()) {
                return {' ', left, nullptr};
            }

            auto right = std::make_shared<MultDivExpr>(ctx.bind(ParseMultDivExpr()));

            return {op.value(), left, right};
        });
    }
};

struct ParsePlusMinusExpr : public Parser<PlusMinusExpr> {
private:
    ParseKnownChar plus {'+'};
    ParseKnownChar minus {'-'};
public:
    ParseResult<PlusMinusExpr> operator()(std::string_view input) const override {
        return monadic_parse<PlusMinusExpr>(input, [this](ParserContext& ctx) -> PlusMinusExpr {
            MultDivExpr left {ctx.bind(ParseMultDivExpr{})};

            std::optional<char> op {ctx.bind(ParseAttempt(ParseFirstMatch<char>({&plus, &minus})))};
            if (!op.has_value())
                return {' ', left, nullptr};

            auto right = std::make_shared<PlusMinusExpr>(ctx.bind(ParsePlusMinusExpr()));

            return {op.value(), left, right};
        });
    }
};

int main() {
    std::string s {"(1,2,3)abc"};
    auto [color, remainder] = ParseRGB{}(s).value();
    std::cout << color.r << ' ' << color.g << ' ' << color.b << '\n';
    std::cout << remainder << '\n';

    std::string s2 {"(1,2,3"};
    assert(!ParseRGB{}(s2).has_value());

    std::string s3 {"(1,2,256)"};
    assert(!ParseRGB{}(s3).has_value()); // Fail based on ctx.fail();

    assert(ParseParseltongue{}("sssssss").value().first == "sssssss");

    ParseKnownChar a_parser('A');
    ParseKnownChar b_parser('B');
    ParseFirstMatch<char> alternative ({&a_parser, &b_parser});

    assert(alternative("A").has_value());
    assert(alternative("B").has_value());
    assert(!alternative("C").has_value());

    assert(ParsePlusMinusExpr{}("1*1+1*1").has_value());

    std::vector<std::string> exprs {
        "2+2",
        "1*2+8/2-3",
        "1+1*4-2",
        "1+1+1+1+1+1+1+1+1+1+1"
    };

    for (std::string s : exprs) {
        auto res {ParsePlusMinusExpr{}(s)};
        assert(res.has_value());
        std::cout << s << " = " << res.value().first.eval() << '\n';
    }

    std::cout << "All tests pass\n";
}
