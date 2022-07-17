
#ifndef jackcasey067_VECTOR_UTILS_H
#define jackcasey067_VECTOR_UTILS_H

#include <vector>

#include "concepts.h"


namespace Util {
    /* Printing Vectors */
        
    /* These globals manipulate the formatting of printed vectors. */
    static bool vector_print_brackets = false;
    static bool vector_print_commas = false;

    /* operator>> is defined on vectors in the global namespace for painless lookup. 
     * See below the Util namespace. */


    /* Reading Vectors */
    
    /* Given a std::istream& `in` and a number of elements to read `n`, returns
     * a std::vector<T> after removing `n` elements from the stream. */
    template <Readable T>
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

template <Util::Printable T>
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
