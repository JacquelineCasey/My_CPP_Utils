
#include "min_heap.h"
#include "vector_utils.h"

#include <array>
#include <cassert>
#include <iostream>
#include <sstream>


struct Order {
public:
    std::string product;
    int cost;

    Order(std::string product, int cost)
        : product {product}, cost {cost} 
    {} 

    bool operator==(const Order& other) const {
        return cost == other.cost && product == other.product;
    } 

    /* Priority based on cost: higher cost -> lower priority -> first in minHeap */
    bool operator<(const Order& other) const {
        return cost > other.cost;
    }
};

template<>
struct std::hash<Order> {
    std::size_t operator()(const Order& order) const {
        return std::hash<string>()(order.product) ^ (hash<int>()(order.cost));    
    }
};

const std::array<Order, 8> orders {
    Order("pants", 15),
    Order("iphone", 900),
    Order("hotdogs", 7),
    Order("elephant", 15000),
    Order("lint", 1),
    Order("Mona Lisa", 1000000000),
    Order("cheese sticks", 13),
    Order("baguette", 10)
};


void test_heapify() {
    Util::MinHeap<Order> q (orders.begin(), orders.end());

    std::vector<std::string> expected {
        "Mona Lisa", "elephant", "iphone", "pants", "cheese sticks", "baguette", "hotdogs", "lint"
    };
    std::vector<std::string> found {};

    while (!q.is_empty()) {
        Order o = q.pop_min();
        found.push_back(o.product);
    }

    assert(expected == found);
}

void test_insert() {
    Util::MinHeap<Order> q {};

    for (const Order& o : orders) {
        q.insert(o);
    }

    std::vector<std::string> expected {
        "Mona Lisa", "elephant", "iphone", "pants", "cheese sticks", "baguette", "hotdogs", "lint"
    };
    std::vector<std::string> found {};

    while (!q.is_empty()) {
        Order o = q.pop_min();
        found.push_back(o.product);
    }

    assert(expected == found);
}

void test_replace_value() {
    /* heapify some */
    Util::MinHeap<Order> q {orders.begin(), &orders[4]};

    /* insert others */
    for (int i {4}; i < static_cast<int>(orders.size()); i++) {
        q.insert(orders[i]);
    }

    /* Update some the the keys */
    q.replace_value(Order("iphone", 900), Order("iphone", 3));
    q.replace_value(Order("baguette", 10), Order("baguette", 70));

    std::vector<std::string> expected {
        "Mona Lisa", "elephant", "baguette", "pants", "cheese sticks", "hotdogs", "iphone", "lint"
    };
    std::vector<std::string> found {};

    while (!q.is_empty()) {
        Order o = q.pop_min();
        found.push_back(o.product);
    }

    std::cout << found << '\n';
    
    assert(expected == found);
}


int main() {
    std::cout << "Testing that creating a new heap sorts the items...\n";
    test_heapify(); 

    std::cout << "Testing that inserting into an existing heap works...\n";
    test_insert(); 

    std::cout << "Testing that update key works...\n";
    test_replace_value();

    std::cout << "TODO Testing that peak_min works...\n";

    std::cout << "TODO Testing that pop_min, peak_min, replace_values, and insert throw useful exception...\n";
}
