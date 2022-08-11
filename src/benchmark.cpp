
#include "benchmark.h"


double Util::benchmark(std::function<void()> func) {
    std::clock_t start = clock();

    func();

    double duration = static_cast<double>(clock() - start) / 1000000;
    return duration;
}
