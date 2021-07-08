#include <iostream>
#include <sstream>
#include <iterator>
#include <zconf.h>
#include "antidebug.hh"


class antidebug_status {
public:
    struct status {
        antidebug::result is_ptrace;
        antidebug::result is_vdso;
        antidebug::result is_noaslr;
        antidebug::result is_env;
        antidebug::result is_ld_preload;
        antidebug::result is_parent_debugger;
        antidebug::result is_ldhook;
        antidebug::result is_nearheap;
        antidebug::result is_valgrind;
        antidebug::result is_perf;
        std::vector<uintptr_t> breakpoints;

        explicit status(antidebug::result r)
        : is_ptrace(r), is_vdso(r), is_noaslr(r), is_env(r),
          is_ld_preload(r), is_parent_debugger(r), is_ldhook(r),
          is_nearheap(r), breakpoints(0), is_valgrind(r),
          is_perf(r) {}

        status()
        : is_ptrace(antidebug::is_ptrace()), is_vdso(antidebug::is_vdso()), is_noaslr(antidebug::is_noaslr()), is_env(antidebug::is_env()),
          is_ld_preload(antidebug::is_ld_preload()), is_parent_debugger(antidebug::is_parent_debugger()), is_ldhook(antidebug::is_ldhook()),
          is_nearheap(antidebug::is_nearheap()), breakpoints(antidebug::get_breakpoints()), is_valgrind(antidebug::is_valgrind()),
          is_perf(antidebug::is_perf()) {}

        friend std::ostream& operator<<(std::ostream& os, const status& s) {
            os << "is_ptrace: " << s.is_ptrace << "\n"
               << "is_vdso: " << s.is_vdso << "\n"
               << "is_noaslr: " << s.is_noaslr  << "\n"
               << "is_env: " << s.is_env << "\n"
               << "is_ld_preload: " << s.is_ld_preload  << "\n"
               << "is_parent_debugger: " << s.is_parent_debugger  << "\n"
               << "is_ldhook: " << s.is_ldhook << "\n"
               << "is_nearheap: " << s.is_nearheap  << "\n"
               << "is_valgrind: " << s.is_valgrind << "\n"
               << "is_perf: " << s.is_perf << "\n"
               << "breakpoints: ";
            for (auto& a : s.breakpoints) {os << (void*)(a) << ","; } os<<"\b";
            return os;
        }
    };

    antidebug_status() = default;

    void update_and_print(std::ostream& os) {
        auto n = status{};
        if (0);
        else if (last.is_ptrace          != n.is_ptrace)     { diff.is_ptrace = antidebug::result::yes;               os << "is_ptrace changed from: " << last.is_ptrace << " to: "<<n.is_ptrace << "\n"; }
        else if (last.is_vdso            != n.is_vdso)       { diff.is_vdso = antidebug::result::yes;                 os << "is_vdso changed from: " << last.is_vdso << " to: "<<n.is_vdso << "\n"; }
        else if (last.is_noaslr          != n.is_noaslr)     { diff.is_noaslr = antidebug::result::yes;               os << "is_noaslr changed from: " << last.is_noaslr << " to: "<<n.is_noaslr << "\n"; }
        else if (last.is_env             != n.is_env)        { diff.is_env = antidebug::result::yes;                  os << "is_env changed from: " << last.is_env << " to: "<<n.is_env << "\n"; }
        else if (last.is_ld_preload      != n.is_ld_preload) { diff.is_ld_preload = antidebug::result::yes;           os << "is_ld_preload changed from: " << last.is_ld_preload << " to: "<<n.is_ld_preload << "\n"; }
        else if (last.is_parent_debugger != n.is_parent_debugger) { diff.is_parent_debugger = antidebug::result::yes; os << "is_parent_debugger changed from: " << last.is_parent_debugger << " to: "<<n.is_parent_debugger << "\n"; }
        else if (last.is_ldhook          != n.is_ldhook)     { diff.is_ldhook = antidebug::result::yes;               os << "is_ldhook changed from: " << last.is_ldhook << " to: "<<n.is_ldhook << "\n"; }
        else if (last.is_nearheap        != n.is_nearheap)   { diff.is_nearheap = antidebug::result::yes;             os << "is_nearheap changed from: " << last.is_nearheap << " to: "<<n.is_nearheap << "\n"; }
        else if (last.is_valgrind        != n.is_valgrind)   { diff.is_valgrind = antidebug::result::yes;             os << "is_valgrind changed from: " << last.is_valgrind << " to: "<<n.is_valgrind << "\n"; }
        else if (last.is_perf            != n.is_perf)       { diff.is_perf = antidebug::result::yes;                 os << "is_perf changed from: " << last.is_perf << " to: "<<n.is_perf << "\n"; }
        else if (last.breakpoints        != n.breakpoints)   { os << "new breakpoints: "; for (auto& a : n.breakpoints) {os << (void*)(a) << ","; } os << "\b\n"; }
        last = n;
    }

public:
    const status init;
    status diff{antidebug::result::no};
    status last;
};

void dummy() {
    std::cout<<" \b"<<std::flush;
}

int main(int argc, char **argv) {
    using namespace std;
    cout<<"pid: "<<getpid()<<endl;
    //https://github.com/yellowbyte/reverse-engineering-reference-manual/blob/master/contents/anti-analysis/Anti-Debugging.md

    antidebug_status ads;
    std::cout<<ads.init<<endl;
    for(size_t i = 0; i<size_t(-1); i++) {
        ads.update_and_print(std::cout);
        usleep(100000);
    }
    dummy();
}
