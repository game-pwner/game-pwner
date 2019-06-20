#include <utility>
#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

using namespace std;


using scan_routine_t = size_t(*)(int,int);

extern scan_routine_t scan_routine;



template<typename _Tp>
size_t MM(_Tp a, _Tp b) {
    cout<<"a-b, where a="<<a<<", b="<<b<<endl;
    return a-b;
}

template<typename _Tp>
size_t PP(_Tp a, _Tp b) {
    cout<<"a+b, where a="<<a<<", b="<<b<<endl;
    return a+b;
}




scan_routine_t get_scanroutine(int __c) {
    if (__c == 0b00) {
        return MM<int>;
    }
    if (__c == 0b10) {
        return PP;
    }
    return nullptr;
}




bool choose_scanroutine(int __c) {
    scan_routine = get_scanroutine(__c);

    return scan_routine != nullptr;
}


int main(int argc, char *argv[]) {
    using namespace std;

    choose_scanroutine(0b00);
    scan_routine(10,20);

    return 0;
}
