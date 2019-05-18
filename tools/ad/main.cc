#include <iostream>
#include <sstream>
#include <zconf.h>
#include "ad.hh"


std::string custom_visual(ssize_t x) {
    std::ostringstream ss;
    switch (x) {
        case -1: ss<<"\033[1;32m"<<"No"<<"\033[0m"; break;
        case 0: ss<<"\033[1;33m"<<"Unknown"<<"\033[0m"; break;
        case 1: ss<<"\033[1;31m"<<"Yes"<<"\033[0m"; break;
    }

    return ss.str();
}

int main(int argc, char *argv[]) {
    using namespace std;
    cout<<"pid: "<<getpid()<<endl;
    //https://github.com/yellowbyte/reverse-engineering-reference-manual/blob/master/contents/anti-analysis/Anti-Debugging.md

    AC ac;
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

    return 0;
}
