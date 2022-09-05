/* Version 2. Templates can be template parameters. 
 * The syntax is a little odd, but this allows us to model Higher Kinded types. 
 * This is great because our main function is no longer littered with primitives. 
 * We let the algorithm decide what the primitives are, and they are all synchronized. 
 * Some code suffers at the margin: 
 * - If you wanted non generic RecursionExtensions, you are shit out of luck. 
 *   You are however free to constrain a generic to only have 1 type. 
 * - The Extension Transformers look a little better, but you have to access a
 *   ::Combined inner class. Luckily, you can use a type alias to make this prettier,
 *   but the type alias requires a template head. */

#include <iostream>
#include <type_traits>
#include <map>


template<typename TReturn, typename ...TArgs>
class RecursionExtension {
public:
    virtual TReturn recursive(TArgs...) = 0;
    
    virtual TReturn recurse(TArgs...) = 0;
};


template<typename TReturn, typename ...TArgs>
class PlainRecursion : public RecursionExtension<TReturn, TArgs...> {
protected:
    virtual TReturn recurse(TArgs... input) {
        return this->recursive(input...);
    }
};


template<typename TReturn, typename ...TArgs>
class MemoizedRecursion : public RecursionExtension<TReturn, TArgs...> {
private:
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


/* We can constrain this type, but we do need it to accept template args at least in theory. */
template<typename TReturn, typename ...TArgs>
    requires std::is_same<long, TReturn>::value
        && std::is_same<int, TArgs...>::value
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

        return ans;
    }
};


template<template <typename Ret, typename ...Args> typename InnerExtension = PlainRecursion>
class CounterRecursionTransformer {
public:
    template<typename TReturn, typename ...TArgs>
        requires std::derived_from<InnerExtension<TReturn, TArgs...>, RecursionExtension<TReturn, TArgs...>>
    class Combined : public InnerExtension<TReturn, TArgs...> {
    private:
        int count = 1;

    protected:
        TReturn recurse(TArgs... args) {
            count++;
            return InnerExtension<TReturn, TArgs...>::recurse(args...);
        }

    public:
        int get_recursion_count() {
            return count;
        }
    };
};


template<typename TReturn, typename ...TArgs>
using CounterRecursion = CounterRecursionTransformer<PlainRecursion>::Combined<TReturn, TArgs...>;


template<template <typename Ret, typename ...Args> typename RE = PlainRecursion> 
    requires std::derived_from<RE<long, int>, RecursionExtension<long, int>>
class Fibonacci : public RE<long, int> {
protected:
    long recursive(int input) final {
        if (input == 0)
            return 0;
        if (input == 1)
            return 1;
        
        return this->recurse(input - 2) + this->recurse(input - 1);
    }

public:
    int fib(int input) {
        return this->recurse(input);
    }
};


template<template<typename Ret, typename ...Args> typename RE = PlainRecursion>
    requires std::derived_from<RE<int, int, int>, RecursionExtension<int, int, int>>
class GCD : public RE<int, int, int> {
protected:
    int recursive(int a, int b) final {
        if (a == 0)
            return b;
        return this->recurse(b % a, a);
    }

public:
    int gcd(int a, int b) {
        return this->recurse(a, b);
    }
};


template<typename TReturn, typename ...TArgs>
using CounterMemoizedRecursion = CounterRecursionTransformer<MemoizedRecursion>::Combined<TReturn, TArgs...>;
// Alternatively, we could write out something like: 
// GCD<CounterRecursionTransformer<MemoizedRecursion>::Combined> memo_counter_gcd {};


int main() {
    std::cout << Fibonacci {}.fib(30) << '\n';

    std::cout << Fibonacci<MemoizedRecursion> {}.fib(45) << '\n';

    std::cout << Fibonacci<LoggedRecursion> {}.fib(6) << '\n';

    Fibonacci<CounterRecursion> counter {};
    std::cout << counter.fib(30) << '\n';
    std::cout << counter.get_recursion_count() << " recursive calls needed. \n";

    Fibonacci<CounterMemoizedRecursion> memoized_counter {};
    std::cout << memoized_counter.fib(45) << '\n';
    std::cout << memoized_counter.get_recursion_count() << " memoized recursive calls needed.\n";

    GCD<CounterMemoizedRecursion> memo_counter_gcd {};
    std::cout << memo_counter_gcd.gcd(168, 144) << '\n';
    std::cout << memo_counter_gcd.get_recursion_count() << " memoized recursive calls needed.\n";
    std::cout << memo_counter_gcd.gcd(168, 144) << '\n';
    std::cout << memo_counter_gcd.get_recursion_count() << " memoized recursive calls needed.\n";
}

/* A few ideas remain for how this might (hypothetically) be used.
 * We could write a recursion extension for testing recursive functions.
 * Also, adding bounds checks to a function could be nice as well.
 * This probably won't work, but some sort of iterator over recursively generated
 * values could be cool.
 * 
 * Sometimes recursion is searching for something, where it does some work and 
 * returns true when it finds a *hit*. Instead of awkwardly fusing the code that
 * rebuilds the answer from this hit into the recursive itself, we could do this
 * via some sort of extender. (Assumes the path could be built from args or 
 * deduced via some paired returns). 
 */
