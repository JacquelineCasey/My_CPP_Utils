
#ifndef jackcasey067_MIN_HEAP_H
#define jackcasey067_MIN_HEAP_H

#include "concepts.h"

#include <concepts>
#include <exception>
#include <vector>
#include <unordered_map>


namespace Util {
    template<typename Value>
        requires Hashable<Value> && std::equality_comparable<Value> && HasLessThan<Value>
    class MinHeap {
    private:
        std::vector<Value> data;
        std::unordered_map<Value, int> val_to_index;

    public:
        MinHeap() 
            : data {std::vector<Value>()}, val_to_index {std::unordered_map<Value, int>()}
        {}

        // O(n)
        template <typename Iterator>
        MinHeap(Iterator begin, Iterator end) : MinHeap() {
            for (Iterator it {begin}; it < end; it++) {
                val_to_index[*it] = data.size(); 
                data.push_back(*it);
            }

            heapify();
        }

        // O(log(n))
        Value pop_min() {
            if (is_empty()) {
                throw std::runtime_error("Tried to pop from empty queue.");
            }

            Value retval {data[0]};
            val_to_index.erase(retval);

            Value last {data.back()};
            data.pop_back();

            if (data.empty()) {
                return retval;
            }

            data[0] = last;
            val_to_index[last] = 0;

            sift_down(0);

            return retval;
        }

        // O(1)
        Value peak_min() {
            if (is_empty()) {
                throw std::runtime_error("Tried to peak from empty queue.");
            }

            return data[0];
        }

        void insert(Value v) {
            if (val_to_index.contains(v))
                throw std::runtime_error("Tried to insert existing error.");

            data.push_back(v);
            val_to_index[v] = data.size() - 1;
            sift_up(data.size() - 1);
        }

        // O(log n)
        void replace_value(Value old, Value updated) {
            if (!val_to_index.contains(old)) {
                throw std::runtime_error("Tried to replace non existent value");
            }

            int index = val_to_index[old];
            data[index] = updated;
            sift_up(index);
            sift_down(index);
        }

        bool is_empty() const {
            return data.empty();
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
                if (data[r] < data[l] && data[r] < data[index]) {
                    swap_at(index, r);
                    sift_down(r);
                } 
                else if (data[l] < data[index]){
                    swap_at(index, l);
                    sift_down(l);
                }
            }
            else if (has_left(index) && data[l] < data[index]) {
                swap_at(index, l);
                sift_down(l);
            }
        }

        // O(n)
        void sift_up(int index) {
            if (!has_parent(index))
                return;

            int p = parent(index);
            if (data[index] < data[p]) {
                swap_at(index, p);
                sift_up(p);
            }
        }

        void swap_at(int a, int b) {
            std::swap(val_to_index[data[a]], val_to_index[data[b]]);
            std::swap(data[a], data[b]);
        }
    };
}

#endif /* jackcasey067_MIN_HEAP_H */
