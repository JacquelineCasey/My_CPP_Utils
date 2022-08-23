
#include "vector_utils.h"

#include <cassert>
#include <sstream>


/* Setup: A custom type with its own << and >> operators. */

class CustomType {
public:
    int x = 0;
    int y = 0;
};

bool operator==(const CustomType& l, const CustomType& r) {
    return (l.x == r.x) && (l.y == r.y);
}

std::ostream& operator<<(std::ostream& out, const CustomType& c) {
    return out << '(' << c.x << ", " << c.y << ')';
}

std::istream& operator>>(std::istream& in, CustomType& c) {
    CustomType c_new {};
    char a;

    in >> a; // (
    assert(a == '(');
    in >> c_new.x;
    in >> a; // ,
    assert(a == ',');
    in >> c_new.y;
    in >> a; // )
    assert(a == ')');
    
    c = c_new;
    return in;
}


/* Static Tests */
static_assert(Util::RecursivelyPrintable<int>);
static_assert(Util::RecursivelyPrintable<std::vector<int>>);
static_assert(Util::RecursivelyPrintable<std::vector<std::vector<CustomType>>>);

static_assert(Util::__Util__Impl::IsVector<std::vector<int>>);
static_assert(Util::RecursivelyReadable<int>);
static_assert(Util::RecursivelyReadable<std::vector<int>>);
static_assert(Util::RecursivelyReadable<std::vector<std::vector<CustomType>>>);

static_assert(!Util::RecursivelyPrintable<std::pair<int, int>>);
static_assert(!Util::RecursivelyReadable<std::pair<int, int>>);

// And the old Printable and Readable do not work, but this is for the better.

// static_assert(!Util::Printable<std::vector<int>>); // VSCode complains erroneously
static_assert(!Util::Readable<std::vector<int>>);

/* Tests */

void test_print_vector() {
    std::stringstream sstream {};
    std::vector<int> v {1, 2, 3};

    sstream << v << '\n';
    assert(sstream.str() == "1 2 3\n");
    sstream.str("");

    Util::vector_print_brackets = true;
    Util::vector_print_commas = true;

    sstream << v << '\n';
    assert(sstream.str() == "[1, 2, 3]\n");
    sstream.str("");

    std::vector<CustomType> v2 {{1, 2}, {3, 4}};
    sstream << v2 << '\n';
    assert(sstream.str() == "[(1, 2), (3, 4)]\n");
    sstream.str("");

    std::vector<std::vector<int>> v3 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    sstream << v3 << '\n';
    assert(sstream.str() == "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]\n");
    sstream.str("");
}

void test_read_vector() {
    std::stringstream sstream {};
    sstream << std::ios_base::in;

    sstream.str("1 2 3");
    auto vec = Util::read_sequence_to_vector<int>(sstream, 3);
    assert((vec == std::vector<int>{1, 2, 3}));
    sstream = {}; // A reset is required, cannot just use .str().

    sstream.str("(1, 2) (3, 4)");
    auto vec2 = Util::read_sequence_to_vector<CustomType>(sstream, 2);
    assert((vec2 == std::vector<CustomType>{{1,2}, {3,4}}));
    sstream = {};
}


int main() {
    std::cout << "Testing Vector Printing...\n";
    test_print_vector();

    std::cout << "Testing Vector Reading...\n";
    test_read_vector();

    // std::vector<std::pair<int, int>> p_vec {{1, 1}, {2, 2}};
    // std::cout << p_vec << '\n';
}
