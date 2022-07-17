
#include "concepts.h"
#include "range.h"

#include <cassert>
#include <iostream>
#include <vector>


/* Tests */

void test_range() {
    Util::Range r1(10);
    std::vector<int> v {};
    std::copy(r1.begin(), r1.end(), std::back_inserter(v));
    assert((v == std::vector{0,1,2,3,4,5,6,7,8,9}));

    Util::Range r2 = Util::Range(5, 10);
    v = {};
    std::copy(r2.begin(), r2.end(), std::back_inserter(v));
    assert((v == std::vector{5,6,7,8,9}));

    Util::Range r3 = Util::Range(0, 10, 2);
    v = {};
    std::copy(r3.begin(), r3.end(), std::back_inserter(v));
    assert((v == std::vector{0,2,4,6,8}));

    Util::Range r4 = Util::Range(1, 10, 2);
    v = {};
    std::copy(r4.begin(), r4.end(), std::back_inserter(v));
    assert((v == std::vector{1,3,5,7,9}));

    Util::Range r5 = Util::Range(1, 11, 2);
    v = {};
    std::copy(r5.begin(), r5.end(), std::back_inserter(v));
    assert((v == std::vector{1,3,5,7,9}));

    /* Test that iterator works in range based for loop */
    int sum {};
    for (int i : Util::Range(1, 6 + 1))
        sum += i;
    assert(sum == 21);
}


int main() {
    std::cout << "Testing Range...\n";
    test_range();
}
