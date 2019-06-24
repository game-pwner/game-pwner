#include <utility>
#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <type_traits>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>
#include <bitmask/bitmask.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libscratch.hh"
using namespace std;


template <size_t CC>
size_t a(size_t x) {
    if constexpr (CC == 1) return 1*x;
    if constexpr (CC == 2) return 10*x;
    return 0;
}

func_t select_predicate(int x) {
    if (x == 1) return a<1>;
    if (x == 2) return a<2>;
    return nullptr;
}
