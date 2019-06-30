//
// Created by root on 28.07.18.
//

#include <iostream>
#include <string>
#include <chrono>
#include <pwner/scanner/Value.hh>
#include <pwner/process/Process.hh>
#include <pwner/process/IO/IOCached.hh>
#include <pwner/wrapper/mm_vector.hh>
#include <pwner/scanner/value/scanner_value.hh>



void test(pwner::Process& proc) {
    using std::cout, std::clog, std::cerr, std::endl;
    using namespace std::chrono;
    using namespace pwner;

    std::string s = "1337";
    // SCANNER::flag flag_wanted{SCANNER::flag_t::i8|SCANNER::flag_t::i16|SCANNER::flag_t::i32|SCANNER::flag_t::i64};
    flag flag_wanted{flag_t::i64};
    user_value uv{s, flag_wanted};
    scanner_value scanner{proc};
    // std::vector<SCANNER::match, mmap_allocator<SCANNER::match>> matches;
    std::vector<match> matches;

    predicate::predicate_t pred = predicate::get_variable_predicate(uv, predicate::variable_predicate::EQUAL_TO);
    {
        Timer __t{"scanning"};
        scanner.scan(matches, uv, pred);
    }
    cout<<"matches: size: "<<matches.size()<<endl;

    for(size_t i=0; i < std::min(matches.size(), size_t{10}); i++) {
        auto& m = matches[i];
        cout<<"m: "<<HEX(m.address)<<", "<<HEX(m.value.u64)<<", "<<m.type<<" \t"<<*proc.get_region(m.address)<<endl;
    }

    {
        Timer __t{"rescanning"};
        scanner.rescan(matches, uv, pred);
    }
    cout<<"matches: size: "<<matches.size()<<endl;

    for(size_t i=0; i < std::min(matches.size(), size_t{10}); i++) {
        auto& m = matches[i];
        cout<<"m: "<<HEX(m.address)<<", "<<HEX(m.value.u64)<<", "<<m.type<<" \t"<<*proc.get_region(m.address)<<endl;
    }
}

int main(int argc, const char *argv[]) {
    oom_score_adj(801);
    using std::cout, std::clog, std::cerr, std::endl;
    using namespace std::chrono;
    using namespace pwner;

    std::string target = "FAKEMEM";
    ProcessProcfs proc{target};
    if (!proc)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");
    clog<<target
        <<", pid: "<<proc.pid()
        <<", cmdline: "<< proc.cmdline()
        <<", exe: "<<proc.exe()
        <<", running: "<<proc.running()
        <<endl;

    test(proc);



    ProcessHeap proc_h{proc};
    test(proc_h);

    ProcessHeap proc_f{proc};
    test(proc_f);

    // clog<<"============================================="<<endl;
    // clog<<"Diff 1 3"<<endl;
    //
    // size_t c = 0;
    // // size_t cc = matches_curr.size();
    // size_t cc = 10;
    // auto fir = matches_first.begin();
    // auto fir_end = matches_first.end();
    // auto curr = matches_curr.begin();
    // auto curr_end = matches_curr.end();
    //
    // while(true) {
    //     if UNLIKELY(fir == fir_end
    //                 && curr == curr_end)
    //         break;
    //     if UNLIKELY(c > cc) {
    //         clog<<"c: "<<c<<endl;
    //         break;
    //     }
    //
    //     if UNLIKELY(curr != curr_end && (*fir).address != (*curr).address) {
    //         c++;
    //         clog<<"lost: "<<*fir;
    //         PROCESS::Region *r = handler->get_region((*fir).address);
    //         if LIKELY(r)
    //             clog<<", region: "<<*r;
    //         clog<<endl;
    //         ++fir;
    //         continue;
    //     }
    //
    //     if LIKELY(curr != curr_end && (*fir).address == (*curr).address) {
    //         if UNLIKELY((*fir).flags != (*curr).flags) {
    //             c++;
    //             clog<<"different: "<<*fir<<" and "<<*curr;
    //             PROCESS::Region *r = handler->get_region((*fir).address);
    //             if LIKELY(r)
    //                 clog<<", region: "<<*r;
    //             clog<<endl;
    //         }
    //         ++fir;
    //         ++curr;
    //         continue;
    //     }
    //     clog<<"wtf"<<endl;
    //     break;
    // }

    return 0;
}
