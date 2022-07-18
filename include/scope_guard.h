
#ifndef jackcasey067_SCOPE_GUARD_H
#define jackcasey067_SCOPE_GUARD_H

#include <functional>
#include <base_classes/noncopyable.h>


namespace Util {
    /* Given an action (anything convertible to std::function<void()>), a ScopeGaurd 
     * calls that action when the ScopeGaurd has its destructor called (usually when
     * it falls out of scope). If dismiss() is called, then the action will not 
     * occur. Useful to guarantee some code to run without repeating yourself in
     * a complex control flow situation, or where exceptions may occur. */
    class ScopeGuard : public NonCopyable {
    public:
        /* Constructs a scope guard. It is likely bad idea to give exception throwing 
         * functions here, but it is not strictly forbidden. */
        ScopeGuard(std::function<void()> func);
        ~ScopeGuard();
        void dismiss();

    private:
        std::function<void()> func;
    };
}

#endif /* jackcasey067_SCOPE_GUARD_H */
