
#ifndef jackcasey067_PARSER_COMBINATORS_H
#define jackcasey067_PARSER_COMBINATORS_H

#include <exception>
#include <optional>
#include <string_view>
#include <vector>


namespace Util::Parser {
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


    /* COMMON PARSERS AND PARSER COMBINATORS */

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
}

#endif /* jackcasey067_PARSER_COMBINATORS_H */
