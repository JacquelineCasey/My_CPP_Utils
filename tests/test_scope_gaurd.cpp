
#include "scope_guard.h"

#include <cassert>
#include <exception>
#include <iostream>


/* Helper Functions */

void queue_scope_guard(std::string& s) {
    Util::ScopeGuard a([&s](){ s = "banana"; });
}

void queue_dismissed_scope_guard(std::string& s) {
    Util::ScopeGuard a([&s](){ s = "banana"; });
    a.dismiss();
}

int func_that_throws() {
    throw std::exception();

    return 5;
}


/* Tests */

void test_scope_gaurds() {
    std::string s {"apple"};
    queue_scope_guard(s);

    assert(s == "banana");
}

void test_dismissed_scope_gaurds() {
    std::string s {"apple"};
    queue_dismissed_scope_guard(s);

    assert(s == "apple");
}

void test_unnamed_block() {
    std::string s {"apple"};

    {
        Util::ScopeGuard([&s]() { s = "banana"; });
    }

    assert(s == "banana");
}

/* To be honest, we might want to change, but then we lose the convenience of writing
 * the lambda as a temporary in most cases (or we do something crazy like a shared_ptr) */
void test_lambda_ownership() {
    std::string s {"apple"};
    int a {0};
    auto lambda {[a, &s]() mutable { // Need this cursed keyword to even test this behavior.
        a++;
        s += std::to_string(a);
    }};

    lambda(); // 1
    lambda(); // 2

    assert(s == "apple12"); // Sanity Check.

    {
        Util::ScopeGuard sg(lambda);

        lambda(); // 3
        lambda(); // 4
    } // ~sg, so 3

    assert(s == "apple12343"); // If lambda captured by reference, "apple12345".
}

void test_reverse_order() {
    std::string s {""};
 
    {
        Util::ScopeGuard([&s]() { s += "a"; });
        Util::ScopeGuard([&s]() { s += "a"; });
        Util::ScopeGuard([&s]() { s += "a"; });
        Util::ScopeGuard([&s]() { s += "a"; });
        Util::ScopeGuard([&s]() { s += "a"; });
    }

    assert(s == "elppa");
}

void test_exception() {
    std::string s {""};

    try {
        Util::ScopeGuard sg ([&s]() { s+= "B";});

        s += "A";
        int a = func_that_throws();
        s += std::to_string(a);
    }
    catch (std::exception e) {
        s += "C";
    }

    assert(s == "ABC");
}


int main() {
    std::cout << "Scope Guards work as expected...\n";
    test_scope_gaurds();

    std::cout << "Dismissed Scope Guards work as expected...\n";
    test_dismissed_scope_gaurds();

    std::cout << "Scope Guards work in unnamed blocks...\n";
    test_unnamed_block();

    std::cout << "Scope Gaurds make copy of lambda...\n";
    test_lambda_ownership();

    std::cout << "Scope Gaurds fire in reverse order of construction...\n";
    test_lambda_ownership();

    std::cout << "Scope Gaurd still fires when exception occurs...\n"; 
    test_exception();
}
