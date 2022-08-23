/* I am inventing something based on the Y combinator that I am calling
 * extendable recursion. (https://en.wikipedia.org/wiki/Fixed-point_combinator#Fixed-point_combinators_in_lambda_calculus)
 * This is more of an exploration of what is possible with the C++ templating system than 
 * anything else...
 * 
 * There is no god here... turn back if you want to retain your sanity...
 */

#include <iostream>
#include <type_traits>
#include <map>

/* Base class for recursion extensions. Recursive, extenable functions will also inherit this (Policy pattern). */
template<typename TReturn, typename ...TArgs>
class RecursionExtension {
public:
    /* The recursive algorithm, declared here so as to be targeted by RecursionExtensions proper. 
     * Implemented in the algorithms that inherit from those extensions. */
    virtual TReturn recursive(TArgs...) = 0;
    
    /* Allow the recursive algorithm to recurse on itself. Extensions will have their special behavior here. */
    virtual TReturn recurse(TArgs...) = 0;
};

/* Generic Extensions may need to redeclare TReturn and ...TArgs */
template<typename TReturn, typename ...TArgs>
class PlainRecursion : public RecursionExtension<TReturn, TArgs...> {
protected:
    /* This is the 'plain' version that does no extension. */
    virtual TReturn recurse(TArgs... input) {
        // https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members (why we need this->)
        return this->recursive(input...);
    }
};

/* Some Extensions need not be generic though. */
template<typename TReturn, typename ...TArgs>
class MemoizedRecursion : public RecursionExtension<TReturn, TArgs...> {
private:
    // Ordered map requires a hash function... But map only needs comparison which tuple gives for free.
    std::map<std::tuple<TArgs...>, TReturn> memo_map {};

protected:
    virtual TReturn recurse(TArgs... input) {
        std::tuple tuple {input...};

        if (memo_map.contains(tuple))
            return memo_map[tuple];
        
        TReturn answer {this->recursive(input...)};
        memo_map[tuple] = answer;
        return answer;
    };
};

/* Demonstrate that some extensions need not be generic */
class LoggedRecursion : public RecursionExtension<long, int> {
private:
    int tabs {0};

    void tab_in() {
        for (int i {0}; i < tabs - 1; i++)
            std::cout << "  | ";
        if (tabs >= 1)
            std::cout << "  |->";
    }

protected:
    virtual long recurse(int input) {
        tab_in();
        std::cout << "recursive(" << input << ")\n";

        tabs++;
        long ans {recursive(input)};
        tabs--;

        // tab_in();
        // std::cout << "recursive(" << input << ") -> " << ans << '\n';
        return ans;
    }
};

/* A terrible fact, but we can chain these if we really want.
 * We had to remove the final keyword from recurse above... which is probably a bad idea. */

// It looks kinda like a monad transformer (if you squint?)
template<typename InnerExtension, typename TReturn, typename ...TArgs>
    requires std::derived_from<InnerExtension, RecursionExtension<TReturn, TArgs...>>
class CounterRecursionTransformer : public InnerExtension { // Impl, since we typedef it to something (slightly) more useable
private:
    int count = 1;

protected:
    TReturn recurse(TArgs... args) {
        count++;
        return InnerExtension::recurse(args...);
    }

/* You can add new functions to children via the policy pattern. */
public:
    int get_recursion_count() {
        return count;
    }
};

/* If we define the transformer first, we can produce the simple version via a typedef.
 * Templatized using statements do exist! */
template<typename TReturn, typename ...TArgs>
using CounterRecursion = CounterRecursionTransformer<PlainRecursion<TReturn, TArgs...>, TReturn, TArgs...>;

/* Example of use */

template<std::derived_from<RecursionExtension<long, int>> RE = PlainRecursion<long, int>>
class Fibonacci : public RE {
protected:
    long recursive(int input) final {
        if (input == 0)
            return 0;
        if (input == 1)
            return 1;
        
        // https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
        return this->recurse(input - 2) + this->recurse(input - 1);
    }

public:
    // Well named entry functions are nice, and can do setup and stuff.
    int fib(int input) {
        return RE::recurse(input); // We should go through RE first.
    }
};

// A contrived example, but lets use multiple arguments.

template<std::derived_from<RecursionExtension<int, int, int>> RE = PlainRecursion<int, int, int>>
class GCD : public RE {
protected:
    int recursive(int a, int b) final {
        if (a == 0)
            return b;
        return RE::recurse(b % a, a);
    }

public:
    int gcd(int a, int b) {
        return RE::recurse(a, b);
    }
};

int main() {
    std::cout << Fibonacci<> {}.fib(30) << '\n';
    // Too slow...
    // std::cout << Fibonacci<> {}.fib(45) << '\n';
    
    // // But now we memoize
    std::cout << Fibonacci<MemoizedRecursion<long, int>> {}.fib(45) << '\n';

    // We can log...
    std::cout << Fibonacci<LoggedRecursion> {}.fib(6) << '\n';

    // Lets try CounterRecursion.
    Fibonacci<CounterRecursion<long, int>> counter {};
    std::cout << counter.fib(30) << '\n';
    std::cout << counter.get_recursion_count() << " recursive calls needed. \n"; // We inherited this!
    // Thats quite a few... no wonder fib(45) stalls.

    // And now, the memoized version...
    /* Yeah, this is a mouthful. We kinda want 'higher kinded types' which might
     * allow us to just pass in MemoizedRecursion (or let Fibonacci handle long and int).
     * on first google, C++ might, somehow, have something like this. An alternative
     * implementation might involve class scoped using declarations to allow access
     * to TArgs and TReturn, somehow */
    Fibonacci<
        CounterRecursionTransformer<
            MemoizedRecursion<long, int>, 
            long, 
            int
        >
    > memoized_counter {};
    std::cout << memoized_counter.fib(45) << '\n'; // Too long unmemoized.
    std::cout << memoized_counter.get_recursion_count() << " memoized recursive calls needed.\n";

    /* The power, of course, comes from plugging in an arbitrary number of arbitrary extensions into 
     * arbitrary algorithms, even if they have arbitrary numbers of arguments and an arbitrary return. */

    GCD<
        CounterRecursionTransformer<
            MemoizedRecursion<int, int, int>, 
            int, 
            int,
            int
        >
    > memo_counter_gcd {};
    std::cout << memo_counter_gcd.gcd(168, 144) << '\n';
    std::cout << memo_counter_gcd.get_recursion_count() << " memoized recursive calls needed.\n";
    std::cout << memo_counter_gcd.gcd(168, 144) << '\n';
    std::cout << memo_counter_gcd.get_recursion_count() << " memoized recursive calls needed.\n";
}

/* Sample Run */

// My_Utils % alias CPPC      
// CPPC='clang++-14  -std=c++20 -Wall -Wextra -Werror -Wpedantic'
// My_Utils % clang++-14 --version
// Homebrew clang version 14.0.6
// Target: x86_64-apple-darwin21.6.0
// Thread model: posix
// InstalledDir: /usr/local/Cellar/llvm/14.0.6_1/bin
// My_Utils % CPPC experimental/extendable_recursion.cpp -o extendableRecursion && ./extendableRecursion
// 832040
// 1134903170
// recursive(6)
//   |->recursive(4)
//   |   |->recursive(2)
//   |   |   |->recursive(0)
//   |   |   |->recursive(1)
//   |   |->recursive(3)
//   |   |   |->recursive(1)
//   |   |   |->recursive(2)
//   |   |   |   |->recursive(0)
//   |   |   |   |->recursive(1)
//   |->recursive(5)
//   |   |->recursive(3)
//   |   |   |->recursive(1)
//   |   |   |->recursive(2)
//   |   |   |   |->recursive(0)
//   |   |   |   |->recursive(1)
//   |   |->recursive(4)
//   |   |   |->recursive(2)
//   |   |   |   |->recursive(0)
//   |   |   |   |->recursive(1)
//   |   |   |->recursive(3)
//   |   |   |   |->recursive(1)
//   |   |   |   |->recursive(2)
//   |   |   |   |   |->recursive(0)
//   |   |   |   |   |->recursive(1)
// 8
// 832040
// 2692538 recursive calls needed. 
// 1134903170
// 90 memoized recursive calls needed.
// 24
// 5 memoized recursive calls needed.
// 24
// 6 memoized recursive calls needed.
// My_Utils % 