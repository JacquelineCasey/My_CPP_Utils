
#ifndef jackcasey067_CONCEPTS_H
#define jackcasey067_CONCEPTS_H

#include <iostream>


namespace Util {
    /* This needs to be visible everywhere, but developers should be discouraged 
     * from touching */
    namespace __Util__Impl {
        /* If we use a function, ADL works a little better. See: 
         * https://stackoverflow.com/questions/72998527/resolving-circular-dependency-between-concept-and-constrained-template-function */
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
        __Util__Impl::print(a);
    };

    template <class T>
    concept Readable = requires(T a) {
        __Util__Impl::read(a);
    };
}

#endif /* jackcasey067_CONCEPTS_H */
