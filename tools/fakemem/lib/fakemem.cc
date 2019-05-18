//
// Created by root on 05.05.19.
//

#include <fakemem.hh>

int x = 2;
int y;

uintptr_t fakelib::MapsMeasure::text() {
    const int a[] = {5,0,1,5,6,7,9,10};
    return reinterpret_cast<uintptr_t>(const_cast<int *>(&a[0]));
}

uintptr_t fakelib::MapsMeasure::data() {
    return reinterpret_cast<uintptr_t>(&x);
}

uintptr_t fakelib::MapsMeasure::rodata() {
    return 0;
}

uintptr_t fakelib::MapsMeasure::bss() {
    return reinterpret_cast<uintptr_t>(&y);
}

uintptr_t fakelib::MapsMeasure::heap() {
    std::vector<int> a;
    a.emplace_back(1);
    return reinterpret_cast<uintptr_t>(&a[0]);
}

uintptr_t fakelib::MapsMeasure::stack() {
    int a = 0;
    return reinterpret_cast<uintptr_t>(&a);
}

std::string fakelib::MapsMeasure::report() {
    std::ostringstream ss;
    uintptr_t t = text();
    uintptr_t d = data();
    uintptr_t r = rodata();
    uintptr_t b = bss();
    uintptr_t h = heap();
    uintptr_t s = stack();
    PWNER::PROCESS::Region *rt = proc.get_region(t);
    PWNER::PROCESS::Region *rd = proc.get_region(d);
    PWNER::PROCESS::Region *rr = proc.get_region(r);
    PWNER::PROCESS::Region *rb = proc.get_region(b);
    PWNER::PROCESS::Region *rh = proc.get_region(h);
    PWNER::PROCESS::Region *rs = proc.get_region(s);
    ss << "text:   " << HEX(t) << ", region: "; if (rt) ss << *rt; ss << "\n";
    ss << "data:   " << HEX(d) << ", region: "; if (rd) ss << *rd; ss << "\n";
    ss << "rodata: " << HEX(r) << ", region: "; if (rr) ss << *rr; ss << "\n";
    ss << "bss:    " << HEX(b) << ", region: "; if (rb) ss << *rb; ss << "\n";
    ss << "heap:   " << HEX(h) << ", region: "; if (rh) ss << *rh; ss << "\n";
    ss << "stack:  " << HEX(s) << ", region: "; if (rs) ss << *rs; ss << "\n";
    return ss.str();
}
