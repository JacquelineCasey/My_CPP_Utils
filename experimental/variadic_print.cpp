
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

int main() {    
    // Hooray!
    println("Some words", 123, 16.66, 123, 123, 123, 123, 123, 456);

    // Bad (fails correctly since p1 is not printable)
    // std::pair<int, int> p1 {1, 2}; // Non printable by default
    // println("Some words", 123, 16.66, 123, 123, 123, 123, 123, p1, 456);
    
    println("First => ", CustomType {}, " Second => ", CustomType {1, 2});
}

/* Link against the library:
 * CPPC experimental/variadic_print.cpp build/my_utils.a -o experimental/vprint -I include/ && ./experimental/vprint*/