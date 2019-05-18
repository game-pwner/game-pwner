/*
    This file is part of Reverse Engine.



    Copyright (C) 2017-2018 Ivan Stepanov <ivanstepanovftw@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>
#include <cstdint>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <sstream>
#include <random>
#include <strings.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <iomanip>
#include <memory>
#include <functional>
#include <exception>          // std::bad_alloc
#include <csignal>
#include <csetjmp>
#include <fstream>
#include <pwner/common.hh>
#include <generators.hh>
#include <fakemem.hh>




template <typename _Type>
class Resource {
public:
    explicit Resource()
    : m_generator(std::make_shared<GeneratorFlat<_Type>>()) { }

    template <typename _Generator>
    void generator() {
        m_generator = std::make_shared<_Generator>();
    }

    bool generate(size_t size) {
        try {
            m_resource.resize(size/sizeof(_Type));
            // m_resource.reserve(size/sizeof(_Type));
        } catch (const std::bad_alloc& e) {
            std::cerr<<"Too much memory requested, resource reset"<<std::endl;
            reset();
            return false;
        }
        m_resource.shrink_to_fit();
        m_generator->fill(m_resource);
        return true;
    }

    // /*! constructor */
    // explicit MemoryGenerator(size_t capacity) noexcept {
    //     memory.reserve(capacity);
    // }
    //
    // /*! copy constructor */
    // MemoryGenerator(const MemoryGenerator& rhs) noexcept
    //         : memory(rhs.memory) { }
    //
    // /*! copy assignment operator */
    // MemoryGenerator& operator=(const MemoryGenerator& rhs) noexcept {
    //     std::swap(memory, rhs.memory);
    //     return *this;
    // }
    //
    // /*! move constructor */
    // MemoryGenerator(MemoryGenerator&& rhs) noexcept
    // : memory(rhs.memory)  { }
    //
    // /*! move assignment operator */
    // MemoryGenerator& operator=(MemoryGenerator&& rhs) noexcept {
    //     std::swap(memory, rhs.memory);
    //     return *this;
    // }

    void reset() {
        // m_resource.resize(0);
        m_resource.clear();
        m_resource.shrink_to_fit();
    }

    static std::string humanReadableByteCount(size_t bytes, bool si = false, int precision = 1) {
        //todo unittest
        using namespace std::string_literals;
        size_t unit = si ? 1000 : 1024;
        if (bytes < unit)
            return std::to_string(bytes) + " B";
        auto exp = static_cast<size_t>(log(static_cast<double>(bytes)) / log(static_cast<double>(unit)));
        exp = std::min(exp, 8uL); // std::max(exp, 0uL) https://www.geeksforgeeks.org/log-function-cpp/ TODO humanReadableByteCount(): может ли такое быть?
        std::string pre = (si ? "kMGTPEZY"s : "KMGTPEZY"s)[exp-1] + (si ? ""s : "i"s);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(precision);
        ss << (static_cast<double>(bytes) / pow(unit, exp)) << " " << pre << "B";
        return ss.str();
    }

    std::string str() const {
        constexpr size_t k = 16;
        size_t v = std::min(m_resource.size(), k);
        size_t b = std::min(m_resource.size() * sizeof(_Type), k);
        std::ostringstream values;
        std::ostringstream bytes;
        for (size_t i = 0; i < v; i++) {
            values << +m_resource[i]; //todo +
            if (i != v - 1)
                values << ", ";
        }
        bytes << std::hex << std::noshowbase << std::setfill('0');
        for (uint8_t *cur = (uint8_t *)(&m_resource[0]); cur < (uint8_t *)(&m_resource[0]) + b; cur++) {
            bytes << "\\x" << std::setw(2) << +*cur;
        }

        std::ostringstream result;
        result << "size: " << humanReadableByteCount(m_resource.size() * sizeof(_Type)) << " (" << m_resource.size() * sizeof(_Type) << ")" << "\n"
               << "length: " << m_resource.size() << "\n"
               << "generator: " << *m_generator.get() << "\n"
               << "resource: " << (void *)(&m_resource[0]) << "\n"
               << "first " << v << " values: ["<<values.str()<< "]" << "\n"
               << "first " << b << " bytes: r'"<<bytes.str()<<"'";
        return result.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const Resource &heap) {
        os << heap.str();
        return os;
    }

    static std::string help() {
        //todo[med]: uglycode: try raw literal
        std::ostringstream result;
        result << "---------------------------------\n"
               << " Command  | Description          \n"
               << "----------+----------------------\n"
               << " > N      | reallocate N bytes\n"
               << " > -N     | reallocate N MiB\n"
               << " > del    | delete an resource\n"
               << " > info   | prints info\n"
               << " > flat   | flat values\n"
               << " > seq    | sequential values\n"
               << " > rand   | random values\n"
               << "---------------------------------";
        return result.str();
    }

public:
    std::shared_ptr<Generator<_Type>> m_generator;
    std::vector<_Type> m_resource;
};


int __variable_global_initialized__ = 1;
int __variable_global_uninitialized__;
const int __array_global_constant_initialized__[4] = {1,2,3,4};
const char *__array_global_constant_initialized2__ = "1234";
const char *__array_global_constant_uninitialized__;
void __function_global__() { }

class MapsMeasure {
public:
    explicit MapsMeasure(PWNER::PROCESS::Process& proc)
    : proc(proc) { }
    std::vector<int> a;


    uintptr_t text() {
        const int __constand[] = {1,2,3,4};
        uintptr_t result = reinterpret_cast<uintptr_t>(&__constand);
        // std::cout<<"text: "<<HEX(result)<<std::endl;
        return result;
    }

    /* The .data segment contains any global or static variables which have a pre-defined value and can be modified */
    uintptr_t data() {
        uintptr_t result = reinterpret_cast<uintptr_t>(&__variable_global_initialized__);
        // std::cout<<"data: "<<HEX(result)<<std::endl;
        return result;
    }

    uintptr_t rodata() {
        uintptr_t result = (uintptr_t)(&__array_global_constant_initialized2__);
        return result;
    }

    uintptr_t bss() {
        uintptr_t result = reinterpret_cast<uintptr_t>(&__variable_global_uninitialized__);
        // std::cout<<"bss: "<<HEX(result)<<std::endl;
        return result;
    }

    uintptr_t heap() {
        a.emplace_back(1);
        uintptr_t result = reinterpret_cast<uintptr_t>(&a[0]);
        // std::cout<<"heap: "<<HEX(result)<<std::endl;
        return result;
    }

    uintptr_t stack() {
        int a = 0;
        uintptr_t result = reinterpret_cast<uintptr_t>(&a);
        // std::cout<<"stack: "<<HEX(result)<<std::endl;
        return result;
    } /// VERIFIED

    std::string report() {
        std::ostringstream ss;
        uintptr_t t = text();
        uintptr_t d = data();
        uintptr_t r = rodata();
        uintptr_t b = bss();
        uintptr_t h = heap();
        uintptr_t s = stack();
        proc.update_regions(); // because heap is changed
        PWNER::PROCESS::Region *rt = proc.get_region(t);
        PWNER::PROCESS::Region *rd = proc.get_region(d);
        PWNER::PROCESS::Region *rr = proc.get_region(r);
        PWNER::PROCESS::Region *rb = proc.get_region(b);
        PWNER::PROCESS::Region *rh = proc.get_region(h);
        PWNER::PROCESS::Region *rs = proc.get_region(s);
        ss << "text:   " << HEX(t) << ", region: "; if (rt) ss << *rt; else ss<<"not found"; ss << "\n";
        ss << "data:   " << HEX(d) << ", region: "; if (rd) ss << *rd; else ss<<"not found"; ss << "\n";
        ss << "rodata: " << HEX(r) << ", region: "; if (rr) ss << *rr; else ss<<"not found"; ss << "\n";
        ss << "bss:    " << HEX(b) << ", region: "; if (rb) ss << *rb; else ss<<"not found"; ss << "\n";
        ss << "heap:   " << HEX(h) << ", region: "; if (rh) ss << *rh; else ss<<"not found"; ss << "\n";
        ss << "stack:  " << HEX(s) << ", region: "; if (rs) ss << *rs; else ss<<"not found"; ss << "\n";
        return ss.str();
    }

public:
    PWNER::PROCESS::Process& proc;
};


int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[], [[maybe_unused]] char *envp[]) {
    oom_score_adj(800);

    PWNER::PROCESS::ProcessProcfs proc(getpid());

    std::cout<<"pid: "<<getpid()<<std::endl;
    MapsMeasure exemm(proc);
    // fakelib::MapsMeasure libmm;
    std::cout<<"exe report:"<<std::endl;
    std::cout<<exemm.report()<<std::endl;

    // std::cout<<"lib report:"<<std::endl;
    // std::cout<<libmm.report()<<std::endl;

    std::cout<<"Maps:"<<std::endl;
    for (const PWNER::PROCESS::Region& r : exemm.proc.regions) {
        std::cout<<r<<std::endl;
    }


    using _Type = unsigned int;
    Resource<_Type> r;

    while (true) {
        std::cout << "> " << std::flush;
        std::string command;
        std::cin >> command;
        if (false) {
        } else if (command.substr(0, 1) == "d") { r.reset(); continue;
        } else if (command.substr(0, 1) == "i") { std::cout<<r<<std::endl; continue;
        } else if (command.substr(0, 1) == "m") {
        } else if (command.substr(0, 1) == "f") { r.generator<GeneratorFlat<_Type>>(); continue;
        } else if (command.substr(0, 1) == "s") { r.generator<GeneratorSequential<_Type>>(); continue;
        } else if (command.substr(0, 1) == "r") { r.generator<GeneratorRandom<_Type>>(); continue;
        } else if (command.substr(0, 1) == "q") { return 0;
        } else {
            ssize_t allocate_please = 0;
            std::istringstream iss(command);
            iss >> allocate_please;
            if (iss.fail()) {
                std::cout << "Please enter an integer." << std::endl;
                continue;
            }
            size_t allocate_that = 0;
            if (allocate_please < 0)
                allocate_that = static_cast<size_t>(-allocate_please * (1u<<20u));
            else
                allocate_that = static_cast<size_t>(allocate_please);
            r.generate(allocate_that);
        }
    }
}
