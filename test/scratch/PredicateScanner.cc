//
// Created by root on 28.07.18.
//

#include <iostream>
#include <string>
#include <chrono>
#include <pwner/scanner/Value.hh>
#include <pwner/process/Process.hh>
#include <pwner/scanner/PredicateScanner.hh>
#include <pwner/wrapper/mm_vector.hh>
#include <pwner/scanner/ValueScanner.hh>


int main(int argc, const char *argv[]) {
    oom_score_adj(801);
    using std::cout, std::clog, std::cerr, std::endl;
    using namespace std::chrono;
    using namespace PWNER;

    std::string target = "FAKEMEM";
    PROCESS::ProcessProcfs proc{target};
    if (!proc)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");
    clog<<target
        <<", pid: "<<proc.pid()
        <<", cmdline: "<< proc.cmdline()
        <<", exe: "<<proc.exe()
        <<", running: "<<proc.running()
        <<endl;

    std::string s = "1337";
    SCANNER::match_type_t mt = SCANNER::match_type_t::MATCHEQUALTO;
    // SCANNER::flag flag_wanted{SCANNER::flag_t::i8|SCANNER::flag_t::i16|SCANNER::flag_t::i32|SCANNER::flag_t::i64};
    SCANNER::flag flag_wanted{SCANNER::flag_t::i16};
    SCANNER::user_value uv{s, flag_wanted};
    SCANNER::ScannerSequential scanner{proc};
    // std::vector<SCANNER::match, mmap_allocator<SCANNER::match>> matches;
    std::vector<SCANNER::match> matches;

    SCANNER::predicate_t pred = SCANNER::get_predicate(mt, uv);
    {
        Timer __t{"scanning"};
        scanner.scan_regions(matches, uv, pred);
    }
    cout<<"matches: size: "<<matches.size()<<endl;

    for(size_t i=0; i < std::min(matches.size(), size_t{10}); i++) {
        auto& m = matches[i];
        cout<<"m: "<<HEX(m.address)<<", "<<HEX(m.value.u64)<<", "<<m.type<<" \t"<<*proc.get_region(m.address)<<endl;
    }

    return 0;
}
