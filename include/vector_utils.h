
#ifndef jackcasey067_VECTOR_UTILS_H
#define jackcasey067_VECTOR_UTILS_H

#include <vector>

#include "concepts.h"


namespace Util {    
    /* Implementation of Concepts */
    namespace __Util__Impl {
        template<typename T>
        concept IsVector = std::is_same<std::vector<typename T::value_type>, T>::value;
    
    
        /* RecursivelyPrintable concept*/

        template <typename T>
        struct RecursivelyPrintable_impl {
            static constexpr bool value = Printable<T>;
        };

        template <__Util__Impl::IsVector T>
        struct RecursivelyPrintable_impl<T> {
            static constexpr bool value = RecursivelyPrintable_impl<typename T::value_type>::value;
        };
 

        /* RecursivelyReadable concept*/

        template <typename T>
        struct RecursivelyReadable_impl {
            static constexpr bool value = Readable<T>;
        };

        template <IsVector T>
        struct RecursivelyReadable_impl<T> {
            static constexpr bool value = RecursivelyReadable_impl<typename T::value_type>::value;
        };
 
        
    }   

    /* Concepts */

    template <typename T>
    concept RecursivelyReadable = __Util__Impl::RecursivelyReadable_impl<T>::value;

    template <typename T>
    concept RecursivelyPrintable = __Util__Impl::RecursivelyPrintable_impl<T>::value;
    

    /* Printing Vectors */
        
    /* These globals manipulate the formatting of printed vectors. */
    static bool vector_print_brackets = false;
    static bool vector_print_commas = false;

    /* operator>> is defined on vectors in the global namespace for painless lookup. 
     * See below the Util namespace. */


    /* Reading Vectors */
    
    /* Given a std::istream& `in` and a number of elements to read `n`, returns
     * a std::vector<T> after removing `n` elements from the stream. */
    template <RecursivelyReadable T>
    std::vector<T> read_sequence_to_vector(std::istream& in, std::size_t n) {
        std::vector<T> vec {};

        for (std::size_t i {}; i < n; i++) {
            T t;
            in >> t;
            vec.push_back(t);
        }

        return vec;
    }
}


/* Printing Vectors (continued) */

template <Util::RecursivelyPrintable T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
    out << (Util::vector_print_brackets ? "[" :  "");

    for (std::size_t i {}; i < vec.size(); i++) {
        out << vec[i];
        if (i < vec.size() - 1) {
            if (Util::vector_print_commas) 
                out << ',';
            out << ' ';
        }
    }

    return out << (Util::vector_print_brackets ? "]" :  "");
}

#endif /* jackcasey067_VECTOR_UTILS_H */
