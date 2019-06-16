//
// Created by root on 28.07.18.
//

#include <iostream>
#include <string>
#include <chrono>
#include <pwner/process/Process.hh>
#include <pwner/scanner/ValueScanner.hh>


int main(int argc, const char *argv[]) {
    oom_score_adj(801);
    using std::cout, std::clog, std::cerr, std::endl;
    using namespace std::chrono;
    using namespace PWNER;

    std::string target = "FAKEMEM";
    std::string search_for = "60";
    // Edata_type data_type = Edata_type::ANYNUMBER;
    Edata_type data_type = Edata_type::INTEGER64;
    std::vector<Cuservalue> uservalue;
    uservalue.resize(1);
    uservalue[0].parse_uservalue_number(search_for);
    Ematch_type match_type = Ematch_type::MATCHEQUALTO;

    std::shared_ptr handler = std::make_shared<PROCESS::ProcessProcfs>(target);
    if (!handler)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");

    ScannerSequential scanner(handler);

    clog<<target
        <<", pid: "<<handler->pid()
        <<", cmdline: "<< handler->cmdline()
        <<", exe: "<<handler->exe()
        <<", running: "<<handler->running()
        <<endl;



    std::vector<value_t> matches_first;
    {
        Timer __t("First Scan");
        scanner.scan_regions(matches_first, data_type, uservalue.data(), match_type);
    }
    clog<<"size: "<<matches_first.size()<<endl;
    clog<<"capacity: "<<matches_first.capacity()<<endl;

    size_t limit = 10;
    clog<<"Printing first "<<limit<<" matches:"<<endl;
    for(auto v : matches_first) {
        if (limit-->0) {
            clog<<v;
            PROCESS::Region *r = handler->get_region(v.address);
            if (r)
                clog<<", region: "<<*r;
            clog<<endl;
            continue;
        }
        clog<<"--- cut ---"<<endl;
        break;
    }

    clog<<"============================================="<<endl;

    std::vector<value_t> matches_prev = matches_first;
    {
        Timer __t("Next Scan");
        {
            Timer __t1("Next Scan/Update");
            scanner.scan_update(matches_prev);
        }
        {
            Timer __t2("Next Scan/Recheck");
            scanner.scan_recheck(matches_prev, data_type, uservalue.data(), match_type);
        }
    }
    clog<<"size: "<<matches_prev.size()<<endl;
    clog<<"capacity: "<<matches_prev.capacity()<<endl;


    clog<<"============================================="<<endl;

    // std::shared_ptr handler_mmap = std::make_shared<PROCESS::ProcessHeap>(*handler);
    std::shared_ptr handler_mmap = std::make_shared<PROCESS::ProcessFile>(*handler, std::filesystem::current_path()/"cg73bskv73pcnrt");
    if (!*handler_mmap)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");
    ScannerSequential scanner_mmap(handler_mmap);


    std::vector<value_t> matches_curr = matches_prev;

    {
        Timer __t("Next Scan");
        {
            Timer __t1("Next Scan/Update");
            scanner_mmap.scan_update(matches_curr);
        }
        {
            Timer __t2("Next Scan/Recheck");
            scanner_mmap.scan_recheck(matches_curr, data_type, uservalue.data(), match_type);
        }
    }
    scanner_mmap.scan_update(matches_curr);
    scanner_mmap.scan_recheck(matches_curr, data_type, uservalue.data(), match_type);
    clog<<"size: "<<matches_curr.size()<<endl;
    clog<<"capacity: "<<matches_curr.capacity()<<endl;

    // return 0;


    clog<<"============================================="<<endl;
    clog<<"Diff 1 3"<<endl;

    size_t c = 0;
    // size_t cc = matches_curr.size();
    size_t cc = 10;
    auto fir = matches_first.begin();
    auto fir_end = matches_first.end();
    auto curr = matches_curr.begin();
    auto curr_end = matches_curr.end();

    while(true) {
        if UNLIKELY(fir == fir_end
                    && curr == curr_end)
            break;
        if UNLIKELY(c > cc) {
            clog<<"c: "<<c<<endl;
            break;
        }

        if UNLIKELY(curr != curr_end && (*fir).address != (*curr).address) {
            c++;
            clog<<"lost: "<<*fir;
            PROCESS::Region *r = handler->get_region((*fir).address);
            if LIKELY(r)
                clog<<", region: "<<*r;
            clog<<endl;
            ++fir;
            continue;
        }

        if LIKELY(curr != curr_end && (*fir).address == (*curr).address) {
            if UNLIKELY((*fir).flags != (*curr).flags) {
                c++;
                clog<<"different: "<<*fir<<" and "<<*curr;
                PROCESS::Region *r = handler->get_region((*fir).address);
                if LIKELY(r)
                    clog<<", region: "<<*r;
                clog<<endl;
            }
            ++fir;
            ++curr;
            continue;
        }
        clog<<"wtf"<<endl;
        break;
    }

    return 0;
}
