#include <string>

struct X {
    std::string s;
    virtual ~X() = default;
};

extern "C" void abracadabra() {
    // if you comment this, it does not crash anymore
    X{};
}
