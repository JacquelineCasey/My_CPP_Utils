
#include "concepts.h"
#include "range.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>


/* Helper function */
void expect_exception(std::function<void()> func) {
    bool b {false};

    try {
        func();
        assert(false);
    }
    catch (Util::Range::InvalidRangeException e) {
        b = true;
    }

    assert(b);
}


/* Tests */

void test_one_arg() {
    Util::Range r1(10);
    std::vector<int> v {};
    std::copy(r1.begin(), r1.end(), std::back_inserter(v));
    assert((v == std::vector{0,1,2,3,4,5,6,7,8,9}));
}

void test_two_args() {
    Util::Range r2 = Util::Range(5, 10);
    std::vector<int> v {};
    std::copy(r2.begin(), r2.end(), std::back_inserter(v));
    assert((v == std::vector{5,6,7,8,9}));
}

void test_three_args() {
    Util::Range r3 = Util::Range(0, 10, 2);
    std::vector<int> v {};
    std::copy(r3.begin(), r3.end(), std::back_inserter(v));
    assert((v == std::vector{0,2,4,6,8}));

    /* Different offset situations still work as well. */ 
    Util::Range r4 = Util::Range(1, 10, 2);
    v = {};
    std::copy(r4.begin(), r4.end(), std::back_inserter(v));
    assert((v == std::vector{1,3,5,7,9}));

    Util::Range r5 = Util::Range(1, 11, 2);
    v = {};
    std::copy(r5.begin(), r5.end(), std::back_inserter(v));
    assert((v == std::vector{1,3,5,7,9}));
}

void test_in_for_loop() {
    int sum {};
    for (int i : Util::Range(1, 6 + 1))
        sum += i;
    assert(sum == 21);
}

void test_backwards() {
    Util::Range r6 = Util::Range(10, 0, -2);
    std::vector<int> v = {};
    std::copy(r6.begin(), r6.end(), std::back_inserter(v));
    assert((v == std::vector{10,8,6,4,2}));
}

void test_exceptions() {
    /* Wrong way, no termination. */
    expect_exception([]() {
        Util::Range r6 = Util::Range(10, 0, 2);
    });

    /* Other wrong way. */
    expect_exception([]() {
        Util::Range r6 = Util::Range(0, 10, -2);
    });
    
    /* Step of 0 */
    expect_exception([]() {
        Util::Range r6 = Util::Range(10, 0, 0);
    });
}


int main() {
    std::cout << "One argument Range works as expected...\n";
    test_one_arg();

    std::cout << "Two argument Range works as expected...\n";
    test_two_args();

    std::cout << "Three argument Range works as expected...\n";
    test_three_args();

    std::cout << "Range works as expected in range-based for loop...\n";
    test_in_for_loop();

    std::cout << "Backwards Range works as expected...\n";
    test_backwards();

    std::cout << "Range throws exceptions for invalid ranges...\n";
    test_exceptions();
}
