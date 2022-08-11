
#ifndef jackcasey067_MIN_HEAP_H
#define jackcasey067_MIN_HEAP_H

#include "concepts.h"

#include <concepts>
#include <exception>
#include <ranges>
#include <vector>
#include <unordered_map>


namespace Util {
    class MinHeapException : std::exception {
        std::string _what;

    public:
        MinHeapException(std::string what) : _what {what} 
        {}

        const char* what() const noexcept override {
            return _what.c_str();
        }
    };

    template<Hashable Value, typename Priority = int>
        requires std::equality_comparable<Value> && HasLessThan<Priority>
    class MinHeap {
    private:
        std::vector<std::pair<Value, Priority>> data;
        std::unordered_map<Value, int> val_to_index;

    public:
        MinHeap() 
            : data {std::vector<std::pair<Value, Priority>>()}, val_to_index {std::unordered_map<Value, int>()}
        {}

        /* Heapify constructors, run in O(n) */

        template<typename Iterator>
            requires std::is_same<std::iter_value_t<Iterator>, std::pair<Value, Priority>>::value
        MinHeap(Iterator begin, Iterator end) : MinHeap() {
            for (Iterator it {begin}; it != end; it++) {
                val_to_index[it->first] = data.size(); 
                data.push_back(*it);
            }

            heapify();
        }

        template<std::ranges::range Range>
        MinHeap(Range r) : MinHeap(r.begin(), r.end())
        {}

        template<typename Iterator>
            requires std::is_same<std::iter_value_t<Iterator>, Value>::value
        MinHeap(Iterator begin, Iterator end, std::function<Priority(const Value&)> priority_function) : MinHeap() {
            for (Iterator it {begin}; it != end; it++) {
                val_to_index[*it] = data.size(); 
                data.push_back(std::pair(*it, priority_function(*it)));
            }

            heapify();
        }

        template<std::ranges::range Range>
        MinHeap(Range r, std::function<Priority(const Value&)> priority_function) : MinHeap (r.begin(), r.end(), priority_function)
        {}

        // O(log(n))
        Value pop_min() {
            if (is_empty()) {
                throw MinHeapException("Tried to pop from empty queue.");
            }

            Value retval {data[0].first};
            val_to_index.erase(retval);

            std::pair<Value, Priority> last {data.back()};
            data.pop_back();

            if (data.empty()) {
                return retval;
            }

            data[0] = last;
            val_to_index[last.first] = 0;

            sift_down(0);

            return retval;
        }

        // O(1)
        Value peak_min() {
            if (is_empty()) {
                throw MinHeapException("Tried to peak from empty queue.");
            }

            return data[0].first;
        }

        // O(log n)
        void insert(Value v, Priority p) {
            if (val_to_index.contains(v))
                throw MinHeapException("Tried to insert existing error.");

            data.push_back({v, p});
            val_to_index[v] = data.size() - 1;
            sift_up(data.size() - 1);
        }

        // O(log n)
        void update_priority(Value v, Priority p_new) {
            if (!val_to_index.contains(v)) {
                throw MinHeapException("Tried to replace non existent value");
            }

            int index = val_to_index[v];
            data[index] = {v, p_new};
            sift_up(index);
            sift_down(index);
        }

        bool is_empty() const {
            return data.empty();
        }

        bool contains(const Value& v) const {
            return val_to_index.contains(v);
        }

        Priority get_priority(const Value& v) {
            if (!contains(v))
                throw MinHeapException("Tried to get priority of nonexistent value");

            return data[val_to_index[v]].second;
        }

        void remove(const Value& v) {
            if (!contains(v))
                throw MinHeapException("Tried to remove nonexistent value");

            int index {val_to_index[v]};
            swap_at(data.size() - 1, index);

            val_to_index.erase(v);
            data.pop_back();

            sift_down(index);
            sift_up(index);
        }

    private:
        int left(int index) const {
            return (index * 2) + 1; 
        }

        int right(int index) const {
            return (index * 2) + 2;
        }

        int parent(int index) const {
            return ((index + 1) / 2) - 1;
        }

        int has_left(int index) const {
            return (index * 2) + 1 < static_cast<int>(data.size());
        }

        int has_right(int index) const {
            return (index * 2) + 2 < static_cast<int>(data.size());
        }

        int has_parent(int index) const {
            return index != 0;
        }

        // Builds heap out of n elements all at once. O(n)
        void heapify() {
            // Iterate backwards over all nodes
            for (int i {static_cast<int>(data.size()) - 1}; i >= 0; i--) {
                sift_down(i);
            }
        }

        // O(log n)
        void sift_down(int index) {
            int l = left(index);
            int r = right(index);
            if (has_right(index)) { // implicitly this also means has_left()
                if (data[r].second < data[l].second && data[r].second < data[index].second) {
                    swap_at(index, r);
                    sift_down(r);
                } 
                else if (data[l].second < data[index].second){
                    swap_at(index, l);
                    sift_down(l);
                }
            }
            else if (has_left(index) && data[l].second < data[index].second) {
                swap_at(index, l);
                sift_down(l);
            }
        }

        // O(n)
        void sift_up(int index) {
            if (!has_parent(index))
                return;

            int p = parent(index);
            if (data[index].second < data[p].second) {
                swap_at(index, p);
                sift_up(p);
            }
        }

        void swap_at(int a, int b) {
            std::swap(val_to_index[data[a].first], val_to_index[data[b].first]);
            std::swap(data[a], data[b]);
        }
    };
}

#endif /* jackcasey067_MIN_HEAP_H */
