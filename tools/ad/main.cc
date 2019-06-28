#include <iostream>
#include <sstream>
#include <zconf.h>
#include "antidebug.hh"


std::string custom_visual(ssize_t x) {
    std::ostringstream ss;
    switch (x) {
        case -1: ss<<"\033[1;32m"<<"No"<<"\033[0m"; break;
        case 0: ss<<"\033[1;33m"<<"Unknown"<<"\033[0m"; break;
        case 1: ss<<"\033[1;31m"<<"Yes"<<"\033[0m"; break;
    }

    return ss.str();
}


class AC_Cached {
public:
    AC_Cached(antidebug ac)
            : ac(ac), is_ptrace(ac.is_ptrace()), is_vdso(ac.is_vdso()), is_noaslr(ac.is_noaslr()), is_env(ac.is_env()),
              is_ld_preload(ac.is_ld_preload()), is_parent_debugger(ac.is_parent_debugger()), is_ldhook(ac.is_ldhook()),
              is_nearheap(ac.is_nearheap()), is_breakpoint(ac.is_breakpoint()), is_valgrind(ac.is_valgrind()),
              is_more_streams(ac.is_more_streams()), is_perf(ac.is_perf()) { }

    ssize_t is_changed() {
        if (0);
        else if (is_ptrace          != ac.is_ptrace())     return 1;
        else if (is_vdso            != ac.is_vdso())       return 1;
        else if (is_noaslr          != ac.is_noaslr())     return 1;
        else if (is_env             != ac.is_env())        return 1;
        else if (is_ld_preload      != ac.is_ld_preload()) return 1;
        else if (is_parent_debugger != ac.is_parent_debugger()) return 1;
        else if (is_ldhook          != ac.is_ldhook())     return 1;
        else if (is_nearheap        != ac.is_nearheap())   return 1;
        else if (is_breakpoint      != ac.is_breakpoint()) return 1;
        else if (is_valgrind        != ac.is_valgrind())   return 1;
        else if (is_more_streams    != ac.is_more_streams()) return 1;
        else if (is_perf            != ac.is_perf())       return 1;
        return -1;
    }

public:
    antidebug ac;
    const ssize_t is_ptrace;
    const ssize_t is_vdso;
    const ssize_t is_noaslr;
    const ssize_t is_env;
    const ssize_t is_ld_preload;
    const ssize_t is_parent_debugger;
    const ssize_t is_ldhook;
    const ssize_t is_nearheap;
    const ssize_t is_breakpoint;
    const ssize_t is_valgrind;
    const ssize_t is_more_streams;
    const ssize_t is_perf;
};


int main(int argc, char *argv[]) {
    using namespace std;
    cout<<"pid: "<<getpid()<<endl;
    //https://github.com/yellowbyte/reverse-engineering-reference-manual/blob/master/contents/anti-analysis/Anti-Debugging.md

    antidebug ac;
    AC_Cached acc(ac);
    acc.is_changed();

    while (true) {
        cout << "is_ptrace: " << custom_visual(ac.is_ptrace()) << endl;
        cout << "is_vdso: " << custom_visual(ac.is_vdso()) << endl;
        cout << "is_noaslr: " << custom_visual(ac.is_noaslr()) << endl;
        cout << "is_env: " << custom_visual(ac.is_env()) << endl;
        cout << "is_ld_preload: " << custom_visual(ac.is_ld_preload()) << endl;
        cout << "is_parent_debugger: " << custom_visual(ac.is_parent_debugger()) << endl;
        cout << "is_ldhook: " << custom_visual(ac.is_ldhook()) << endl;
        cout << "is_nearheap: " << custom_visual(ac.is_nearheap()) << endl;
        cout << "is_breakpoint: " << custom_visual(ac.is_breakpoint()) << endl;
        cout << "is_valgrind: " << custom_visual(ac.is_valgrind()) << endl;
        cout << "is_more_streams: " << custom_visual(ac.is_more_streams()) << endl;
        cout << "is_perf: " << custom_visual(ac.is_perf()) << endl;
        // if (acc.is_changed() != -1) {
        //     clog<<"Banned"<<endl;
        //     return 1;
        // }
        sleep(1);
    }

    return 0;
}
