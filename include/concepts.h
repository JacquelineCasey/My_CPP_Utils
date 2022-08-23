
#ifndef jackcasey067_CONCEPTS_H
#define jackcasey067_CONCEPTS_H

#include <iostream>


namespace Util {
    /* This needs to be visible everywhere, but developers should be discouraged 
     * from touching */
    namespace __Util__Impl {
        /* If we use a function, ADL works a little better. See: 
         * https://stackoverflow.com/questions/72998527/resolving-circular-dependency-between-concept-and-constrained-template-function 
         * UPDATE: this doesn't actually work */
        template <class T>
        void print(T a) {
            std::cout << a;
        }

        template <class T>
        void read(T a) {
            std::cin >> a;
        }
    }

    template <class T>
    concept Printable = requires(T a) {
        // __Util__Impl::print(a);
        std::cout << a;
    };

    template <class T>
    concept Readable = requires(T a) {
        // __Util__Impl::read(a);
        std::cin >> a;
    };


    /* Hashable */
    template<typename T>
    concept Hashable = requires(T a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };

    // /* Has an equality operator. */ use std::equality_comparable<T>
    // template<typename T>
    // concept HasEquality = requires(const T a, const T b) {
    //     { a == b } -> std::convertible_to<bool>;
    // };

    template<typename T>
    concept HasLessThan = requires(const T a, const T b) {
        { a < b } -> std::convertible_to<bool>;
    };
}

#endif /* jackcasey067_CONCEPTS_H */
