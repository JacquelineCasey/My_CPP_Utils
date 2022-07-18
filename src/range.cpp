
#include "range.h"


namespace Util {
    
    using RangeIterator = Range::RangeIterator;
    using InvalidRangeException = Range::InvalidRangeException;


    /* InvalidRangeException */

    const char* InvalidRangeException::what() const noexcept {
        return "Range had invalid parameters and would not terminate.";
    }


    /* RangeIterator */

    RangeIterator::RangeIterator(int num, int step) : num {num}, step {step} {}

    RangeIterator::RangeIterator() : RangeIterator(0, 1) {}

    bool RangeIterator::operator==(const RangeIterator& other) const {
        return num == other.num;
    }

    bool RangeIterator::operator!=(const RangeIterator& other) const {
        return !(*this == other);
    }

    int RangeIterator::operator*() const {
        return num;
    }

    RangeIterator& RangeIterator::operator++() { // pre-increment
        num += step;
        return *this;
    }

    RangeIterator RangeIterator::operator++(int) { // post-increment
        RangeIterator ret {*this};
        num += step;
        return ret;
    }

    RangeIterator& RangeIterator::operator--() { // pre-decrement
        num -= step;
        return *this;
    }

    RangeIterator RangeIterator::operator--(int) { // post-decrement
        RangeIterator ret {*this};
        num -= step;
        return ret;
    }

    // Checks that the iterator defines everything necessary.
    static_assert(std::bidirectional_iterator<RangeIterator>); // Could update to random.


    /* Range (helper function) */

    int determine_stop(int start, int stop, int step) { // private helper function
        if (((stop - start) % step) == 0)
            return stop;
                
        return stop - ((stop - start) % step) + step;
    }


    /* Range (api functions) */

    Range::Range(int start , int stop, int step)
            : start {start}, stop {determine_stop(start, stop, step)}, step {step} {
        if ((step == 0)
            || (step < 0 && stop - start > 0)
            || (step > 0 && stop - start < 0)) {
            throw InvalidRangeException();
        }  
    }

    Range::Range(int start, int stop) : Range(start, stop, 1) {}

    Range::Range(int stop) : Range(0, stop, 1) {}

    RangeIterator Range::begin() {
        return RangeIterator(start, step);
    }

    RangeIterator Range::end() {
        return RangeIterator(stop, step);
    }
}
