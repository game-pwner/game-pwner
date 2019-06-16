#include <iostream>
#include <cstring>
#include <bitset>
#include <memory>
#include <cmath>
#include <iomanip>
#include <vector>
#include <memory>

using namespace std;


class base {
public:
    virtual ~base() = default;
    virtual void none() const = 0;
};

class der1 : public base {
public:
    der1() = default;
    operator bool() {return true;}
    void none() const {cout<<"123"<<endl;}
};

class my {
public:
    // my(shared_ptr<base> x)
    // : parent(std::move(x)) {}
    //
    // my(base& x) {
    //     parent = shared_ptr<base>(&x, []([[maybe_unused]] base *p){});
    // }

    my(const shared_ptr<base>& x)
    : parent(*x) {parent.none();}

    my(const base& x)
    : parent(const_cast<base&>(x)) {parent.none();}

    void gav() {parent.none();}

    // std::shared_ptr<base> parent;
    const base& parent;
};



int main() {
    using namespace std;

    my *m;
    {
        der1 d1;
        m = new my(d1);
    }
    m->gav();
    delete m;

    return 0;
}