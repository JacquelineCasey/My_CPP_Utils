
#include "min_heap.h"
#include "range.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <random>
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
};

template<>
struct std::hash<Order> {
    std::size_t operator()(const Order& order) const {
        return std::hash<string>()(order.product) ^ (hash<int>()(order.cost));    
    }
};

// Negative priorities ensure minimum is largest or most important order.
const std::array<Order, 8> orders {
    Order("pants", 15),
    Order("iphone", 900),
    Order("hotdogs", 7),
    Order("elephant", 15000),
    Order("lint", 1),
    Order("Mona Lisa", 1000000000),
    Order("cheese sticks", 13),
    Order("baguette", 10),
};

// Negative priorities ensure minimum is largest or most important order.
const std::array<std::pair<Order, int>, 8> orders_and_priority {
    std::pair{Order("pants", 15), -15},
    std::pair{Order("iphone", 900), -900},
    std::pair{Order("hotdogs", 7), -7},
    std::pair{Order("elephant", 15000), -15000},
    std::pair{Order("lint", 1), -1},
    std::pair{Order("Mona Lisa", 1000000000), -1000000000},
    std::pair{Order("cheese sticks", 13), -13},
    std::pair{Order("baguette", 10), -10}
};


void test_heapify() {
    Util::MinHeap<Order> q (orders, [](const Order& o) {
        return -o.cost; // Negative cost ensures that high costs come out first.
    });

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

    for (const auto& [order, priority] : orders_and_priority) {
        q.insert(order, priority);
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
    Util::MinHeap<Order> q {orders_and_priority.begin(), &orders_and_priority[4]};

    /* insert others */
    for (int i {4}; i < static_cast<int>(orders_and_priority.size()); i++) {
        q.insert(orders_and_priority[i].first, orders_and_priority[i].second);
    }

    /* Update some the the keys */
    q.update_priority(Order("iphone", 900), -3);
    q.update_priority(Order("baguette", 10), -70);

    std::vector<std::string> expected {
        "Mona Lisa", "elephant", "baguette", "pants", "cheese sticks", "hotdogs", "iphone", "lint"
    };
    std::vector<std::string> found {};

    while (!q.is_empty()) {
        Order o = q.pop_min();
        found.push_back(o.product);
    }
    
    assert(expected == found);
}

void test_peak_min() {
    Util::MinHeap<Order> q (orders, [](const Order& o) {
        return -o.cost; // Negative cost ensures that high costs come out first.
    });

    assert(q.peak_min().product == "Mona Lisa");
    assert(q.peak_min().product == "Mona Lisa");
    assert(q.peak_min().product == "Mona Lisa");

    q.insert(Order("2 Mona Lisas", 2 * q.peak_min().cost), - 2 * q.peak_min().cost);

    assert(q.peak_min().product == "2 Mona Lisas");
    assert(q.peak_min().product == "2 Mona Lisas");
    assert(q.peak_min().product == "2 Mona Lisas");
}

void test_exceptions() {
    Util::MinHeap<int> q {};

    int errors_caught {0};
    
    try {
        q.peak_min();    
    } catch (Util::MinHeapException e) {
        errors_caught++;
    }

    try {
        q.pop_min();    
    } catch (Util::MinHeapException e) {
        errors_caught++;
    }

    try {
        q.update_priority(1, 5);    
    } catch (Util::MinHeapException e) {
        errors_caught++;
    }

    q.insert(2, 2);

    try {
        q.insert(2, 3);    
    } catch (Util::MinHeapException e) {
        errors_caught++;
    }

    assert(errors_caught == 4);
}

void test_large_shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());

    Util::Range r(300000);
    std::vector<int> vec (r.begin(), r.end());
    std::shuffle(vec.begin(), vec.end(), g);

    Util::MinHeap<int> q (vec, [](int i){
        return i;
    });

    Util::MinHeap<int> q2 {};

    for (int i {0}; i < 1000; i++) {
        assert(q.pop_min() == i);
        q2.insert(i, i);
    }

    for (int i {0}; i < 1000; i+=2) {
        q2.update_priority(i, -i);
    }

    for (int i {1}; i < 1000; i+=2) {
        q2.update_priority(i, 3*i); // Increasing priority should also be tested.
    }

    for (int i {0}; i < 1000; i+=7) {
        q2.remove(i);
    }

    for (int i {998}; i >= 0; i-=2) {
        if (i % 7 == 0)
            continue;
        assert(i == q2.pop_min());
        if ((i + 1) % 7 != 0) {
            assert(q2.contains(i + 1)); // We'll also test contains
            assert(q2.get_priority(i+1) == 3 * (i+1)); // And get_priority
        }
    }

    for (int i {1}; i < 1000; i+=2) {
        if (i % 7 == 0)
            continue;
        assert(i == q2.pop_min());
    }
}

void test_remove() {
    Util::MinHeap<Order> q (orders, [](const Order& o) {
        return -o.cost; // Negative cost ensures that high costs come out first.
    });

    std::vector<std::string> expected {
        "Mona Lisa", "elephant", "iphone", "cheese sticks", "baguette", "hotdogs", "lint"
    };
    std::vector<std::string> found {};

    q.remove(Order("pants", 15));

    while (!q.is_empty()) {
        Order o = q.pop_min();
        found.push_back(o.product);
        assert(o.product != "pants");
    }

    assert(expected == found);
}


int main() {
    std::cout << "Testing that creating a new heap sorts the items...\n";
    test_heapify(); 

    std::cout << "Testing that inserting into an existing heap works...\n";
    test_insert(); 

    std::cout << "Testing that update key works...\n";
    test_replace_value();

    std::cout << "Testing that peak_min works...\n";
    test_peak_min();

    std::cout << "Testing that pop_min, peak_min, update_priority, and insert throw useful exception...\n";
    test_exceptions();

    std::cout << "Testing that the heap works with large inputs...\n";
    test_large_shuffle();

    std::cout << "Test removal of element...\n";
    test_remove();
}
