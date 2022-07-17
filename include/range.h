

#ifndef jackcasey067__RANGE_H
#define jackcasey067__RANGE_H

#include <exception>
#include <iterator>


namespace Util {
    class Range {
    public:
        class InvalidRangeException : public std::exception {
        public:
            virtual const char* what() const noexcept;
        };
        
        /* Currently a biderectional_iterator, which is weaker than a random 
         * access iterator. */
        class RangeIterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = int;
            using value_type        = int;
            using pointer           = int*;
            using reference         = int&;

            RangeIterator(int num, int step);
            RangeIterator();

            bool operator==(const RangeIterator& other) const;
            bool operator!=(const RangeIterator& other) const;
            
            int operator*() const;

            RangeIterator& operator++();
            RangeIterator operator++(int);

            RangeIterator& operator--();
            RangeIterator operator--(int);
        
        private:
            int num;
            int step;
        };

    public:
        Range(int start , int stop, int step);
        Range(int start, int stop);
        Range(int stop);

        RangeIterator begin();
        RangeIterator end();

    private:
        const int start;
        const int stop;
        const int step;
    };
}

#endif /* jackcasey067__RANGE_H */