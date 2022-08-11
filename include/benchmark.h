
#ifndef jackcasey067_BENCHMARK_H
#define jackcasey067_BENCHMARK_H

#include <functional>


namespace Util {
    double benchmark(std::function<void()> func);
}

#endif /* jackcasey067_BENCHMARK_H */
