#include <vector>

struct X {
    std::vector<int> v;
    virtual ~X() = default;
    virtual void f() {}
};

extern "C" void Open() {
    X *x = new X();
    x->f();
    delete x;
}
