
#include <base_classes/noncopyable.h>

#include <cassert>
#include <concepts>
#include <iostream>


class DummyClass : public NonCopyable {
public:
    int x;

    DummyClass(int x) : x{x} {}

    DummyClass(DummyClass&& other) : x{other.x} {}
    
    DummyClass& operator=(DummyClass&& other) {
        x = other.x;
        return *this;
    }
};


/* Static asserts to ensure that DummyClass is in no way copyable. */

static_assert(!std::copyable<DummyClass>);
static_assert(!std::constructible_from<DummyClass, DummyClass&>);
static_assert(!std::constructible_from<DummyClass, const DummyClass&>);
static_assert(!std::constructible_from<DummyClass, const DummyClass>);
static_assert(!std::assignable_from<DummyClass&, DummyClass&>);
static_assert(!std::assignable_from<DummyClass&, const DummyClass&>);
static_assert(!std::assignable_from<DummyClass&, const DummyClass>);


/* Runtime tests */

/* Check that you can still make a class movalbe. */
void test_noncopyable() {
    DummyClass a {2};
    DummyClass b {4};
    a = std::move(4);
    assert(a.x == 4);

    DummyClass c {std::move(a)};
    assert(c.x == 4);
}


int main() {
    std::cout << "Testing NonCopyable...\n";
    test_noncopyable();
}
