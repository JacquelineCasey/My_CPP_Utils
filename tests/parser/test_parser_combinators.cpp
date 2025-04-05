
#include "parser.h"

#include <cassert>
#include <iostream>

using namespace Util::Parser;

/* Some parsers written in using the library. */

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

    // for fun we will also convert expressions to reverse polish notation,
    // which is noteworthy for allowing us to compute a value simply by reading
    // symbols as stack instructions (common bytecodes use a such a stack architecture).
    std::string to_rpn() {
        if (op == ' ')
            return std::to_string(left);
        
        return std::to_string(left) + ' ' + right->to_rpn() + ' ' + op;
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

    std::string to_rpn() {
        if (op == ' ')
            return left.to_rpn();
        
        return left.to_rpn() + ' ' + right->to_rpn() + ' ' + op;
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

/* Tests of the above parsers */

void test_color_parser() {
    std::string s {"(1,2,3)abc"};
    auto [color, remainder] = ParseRGB{}(s).value();
    assert(color.r == 1 && color.g == 2 && color.b == 3);
    assert(remainder == "abc");

    std::string s2 {"(1,2,3"};
    assert(!ParseRGB{}(s2).has_value());

    std::string s3 {"(1,2,256)"};
    assert(!ParseRGB{}(s3).has_value()); // Fail based on ctx.fail();
}

void test_greedy_parser() {
    assert(ParseParseltongue{}("sssssss").value().first == "sssssss");
}

// It is kinda like Parsec's predictive <|>. Kinda
void test_alternative_parser() {
    ParseKnownChar a_parser('A');
    ParseKnownChar b_parser('B');
    ParseFirstMatch<char> alternative ({&a_parser, &b_parser});

    assert(alternative("A").has_value());
    assert(alternative("B").has_value());
    assert(!alternative("C").has_value());
}

void test_math_expr_parser() {
    assert(ParsePlusMinusExpr{}("1*1+1*1").has_value());

    std::vector<std::string> exprs {
        "2+2",
        "1*2+8/2-3",
        "1+1*4-2",
        "1+1+1+1+1+1+1+1+1+1+1"
    };

    std::vector<int> answers {
        4, 3, 3, 11
    };

    std::vector<std::string> rpn {
        "2 2 +",
        "1 2 * 8 2 / 3 - +",
        "1 1 4 * 2 - +",
        "1 1 1 1 1 1 1 1 1 1 1 + + + + + + + + + +"
    };

    // Nota Bene - I'm not sure we treat expressions like 1 - 3 + 5 correctly. We might get -7...

    for (size_t i {0}; i < 4; i++) {
        auto res {ParsePlusMinusExpr{}(exprs[i])};
        assert(res.has_value());
        assert(res.value().first.eval() == answers[i]);
        assert(res.value().first.to_rpn() == rpn[i]);
    }
}

int main() {
    std::cout << "Testing rgb color parser...\n";
    test_color_parser();

    std::cout << "Testing greedy parser...\n";
    test_greedy_parser();

    std::cout << "Testing alternative parser...\n"; 
    test_alternative_parser();

    std::cout << "Testing math expression parser (+-*/)...\n";
    test_math_expr_parser();
}