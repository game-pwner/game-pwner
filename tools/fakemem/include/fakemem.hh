
#pragma once

#include <pwner/process/Process.hh>
#include <cstdint>

namespace fakelib {

class MapsMeasure {
public:
    MapsMeasure()
    : proc(getpid()) { }

    uintptr_t text();
    uintptr_t data();
    uintptr_t rodata();
    uintptr_t bss();
    uintptr_t heap();
    uintptr_t stack();

    std::string report();
public:
    PWNER::PROCESS::ProcessProcfs proc;
};

}