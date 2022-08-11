
#include "benchmark.h"
#include "min_heap.h"
#include "range.h"

#include <cassert>
#include <iostream>


void test_basic() {
    int out;
    double time1 = Util::benchmark([&out]() {
        Util::MinHeap<int, long> q (Util::Range(100000), [](int i){
            return (i * 3203l) % 7057l; // some 4 digit primes
        });

        out = q.size();
    });

    assert(out == 100000); // Demonstrates how to get output from a void function.

    double time2 = Util::benchmark([]() {
        Util::MinHeap<int, long> q (Util::Range(200000), [](int i){
            return (i * 3203l) % 7057l; // some 4 digit primes
        });
    });

    assert(time1 < time2);
} 

int main() {
    std::cout << "TODO testing benchmark...\n";
    test_basic();
}
