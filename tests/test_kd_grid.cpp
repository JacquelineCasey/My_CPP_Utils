
#include "kd_grid.h"

#include <cassert>
#include <iostream>


void test_basic() {
    Util::KDGrid<int, 3> grid3 {{10, 12, -10, 0, 0, 5}}; // 3D grid. You pass bounds and an (optional) default value.

    for (int i {10}; i <= 12; i++) {
        for (int j {-10}; j <= 0; j++) {
            for (int k {0}; k <= 5; k++) {
                assert((grid3[{i, j, k}] == 0));
                grid3[{i, j, k}] = (i + j + k) % 10;
                // std::cout << grid3[{i, j, k}] << ' ';
                assert((grid3[{i, j, k}] == (i + j + k) % 10));
            }
            // std::cout << '\n';
        }
        // std::cout << '\n';
    }

    // Handles non int inner types, and other sizes...

    Util::KDGrid<std::string, 5> string_grid {{-10, 10, -10, 10, -10, 10, -10, 10, -10, 10}, "Hello World"};

    for (int i {-10}; i < 10; i++) {
    for (int j {-10}; j < 10; j++) {
    for (int k {-10}; k < 10; k++) {
    for (int l {-10}; l < 10; l++) {
    for (int m {-10}; m < 10; m++) {
        assert((string_grid[{i, j, k, l, m}] == "Hello World"));
        string_grid[{i, j, k, l, m}] = "!!!";
        assert((string_grid[{i, j, k, l, m}] == "!!!"));
    }}}}}

    // The other indices are implicit zeros. This is not intentional, but I suppose it is fine.
    string_grid[{1, 2, 3}] = "Goodbye";
    assert((string_grid[{1,2,3,0,0}] == "Goodbye"));
}

int main() {
    std::cout << "Basic Test...\n";
    test_basic();
}
