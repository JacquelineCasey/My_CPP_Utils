/*
 * kd_grid.h
 *
 * A template for a k dimension grid containing any data type.
 * 
 * TODO: - Better access to bounds. Better iteration? Drawing prisms of various dimension.
 */
#ifndef jackcasey067_KD_GRID_H
#define jackcasey067_KD_GRID_H

#include <array>
#include <string>
#include <vector>

namespace Util {
    namespace __Util__Impl {
        /* Thus begins the template magic (or template madness). */

        /* Used to issue a compiler error in an else after constexpr if. */
        template<class T> struct dependent_false : std::false_type {};

        template<typename T, typename K> // K is a std::integral_constant
        class KVec {
        private:
            std::vector<KVec<T, std::integral_constant<int, K::value - 1>>> vec {};

        public:
            void init(std::array<int, K::value * 2> bounds, T default_value) {
                int lower_bound {bounds[0]};
                int upper_bound {bounds[1]};

                if constexpr (K::value == 1) {
                    vec = std::vector(upper_bound - lower_bound + 1, KVec<T, std::integral_constant<int, 0>>(default_value));
                }
                else if constexpr (K::value > 1) {
                    vec = std::vector(upper_bound - lower_bound + 1, KVec<T, std::integral_constant<int, K::value-1>> {});

                    std::array<int, (K::value - 1) * 2> next_bounds;
                    for (int i {0}; i < K::value * 2 - 2; i++) {
                        next_bounds[i] = bounds[i+2];
                    }

                    for (int i {lower_bound}; i <= upper_bound; i++) {
                        vec[i - lower_bound].init(next_bounds, default_value);
                    }
                }
                else {
                    static_assert(dependent_false<K>::value, "Invalid value of K in KVec (in KDGrid).");
                }
            }

            T& get(const std::array<int, K::value>& indices) {
                int index {indices[0]};
                if constexpr (K::value == 1) {
                    return vec.at(index).item;
                }
                else if constexpr (K::value > 1) {
                    std::array<int, K::value - 1> next_indices;
                    for (int i {0}; i < K::value - 1; i++) {
                        next_indices[i] = indices[i+1];
                    }

                    return vec.at(index).get(next_indices);
                }
                else {
                    static_assert(dependent_false<K>::value, "Invalid value of K in KVec (in KDGrid).");
                }
            }
        };
        
        template<typename T> 
        class KVec<T, std::integral_constant<int, 0>> {
        public:
            T item;
            KVec(T item) : item {item} {}
        };
    }

    template<typename T, int K>
    class KDGrid {
    private:
        const std::array<int, K*2> bounds; // min1, max1, min2, max2, ...

         /* Think of this as K std::vectors nested around the type T. */
        __Util__Impl::KVec<T, std::integral_constant<int, K>> matrix {};

    public:
        static constexpr int dimensions {K};
       
        /* Takes an array of the inclusive bounds in order. Eg {{-10, 10, -10, 10}}*/
        KDGrid(std::array<int, K*2> bounds, T default_value) : bounds {bounds} {
            matrix.init(bounds, default_value);
        }

        KDGrid(std::array<int, K*2> bounds) requires std::default_initializable<T>
            : KDGrid(bounds, {}) {}

        T& operator[](std::array<int, K> indices) {
            for (int k {0}; k < dimensions; k++) {
                if (indices[k] < bounds[2 * k] || indices[k] > bounds[2 * k+1]) {
                    throw std::out_of_range("KDGrid: Length Error. In the " + std::to_string(k + 1) + "'th dimension, tried to reach index " 
                        + std::to_string(indices[k]) + " but min is " + std::to_string(bounds[2 * k]) + " and max is " + std::to_string(bounds[2 * k+1]));
                }
                // We handle the index bump here.
                indices[k] -= bounds[2 * k];
            }
            return matrix.get(indices);
        }
    };
}

#endif /* jackcasey067_FAST_IO_H */
