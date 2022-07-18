
#include "scope_guard.h"


namespace Util {
    ScopeGuard::ScopeGuard(std::function<void()> func) : func{func} {}

    ScopeGuard::~ScopeGuard() {
        func();
    }

    void ScopeGuard::dismiss() {
        /* Simply replace with an empty lambda. */
        func = [](){};
    }
}
