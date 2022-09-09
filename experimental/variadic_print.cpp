
#include "concepts.h"

#include <iostream>


void println() {
    std::cout << '\n';
}

template<Util::Printable First, Util::Printable ...Rest>
void println(const First& first, Rest... rest) {
    std::cout << first << " ";
    println(rest...);
}


class CustomType {
public:
    int a {10};
    int b {20};

public:
    friend std::ostream& operator<<(std::ostream& out, const CustomType& c) {
        return out << "{a: " << c.a << ", b:  " << c.b << "}";
    }
};


/* I stumbled across this recently: 
 * https://www.reddit.com/r/ProgrammerHumor/comments/x95019/comment/inng62x/?utm_source=share&utm_medium=web2x&context=3 */

/* You can do parameter pack expansion via this special ... pattern syntax.
 * so this becomes:: (std::cout << std::forward<T1>(t1), std::cout << std::forward<T2>(t2)...) 
 *
 * Its also using std::forward to preserve the value category of the input. */
template <typename ...Ts>
void print_2(Ts &&...ts) {
    ((std::cout << std::forward<Ts>(ts)), ...) << std::endl;
}

int main() {    
    // Hooray!
    println("Some words", 123, 16.66, 123, 123, 123, 123, 123, 456);

    // Bad (fails correctly since p1 is not printable)
    // std::pair<int, int> p1 {1, 2}; // Non printable by default
    // println("Some words", 123, 16.66, 123, 123, 123, 123, 123, p1, 456);
    
    println("First => ", CustomType {}, " Second => ", CustomType {1, 2});

    print_2(120, " ", "hello ", CustomType {});
}


/* Link against the library:
 * CPPC experimental/variadic_print.cpp build/my_utils.a -o experimental/vprint -I include/ && ./experimental/vprint */


