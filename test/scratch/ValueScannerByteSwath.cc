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
    high_resolution_clock::time_point timestamp;

    //sleep(100000);
    //return 0;

    std::string target = "FAKEMEM";
    // std::string target = "csgo_linux64";
    std::string search_for = "60";
    // PWNER::Edata_type data_type = PWNER::Edata_type::ANYNUMBER;
    PWNER::Edata_type data_type = PWNER::Edata_type::INTEGER64;

    PWNER::PROCESS::ProcessProcfs handler(target);
    if (!handler)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");

    PWNER::ScannerByteSwath scanner(handler);

    clog<<target
        <<", pid: "<<handler.pid()
        <<", cmdline: "<< handler.cmdline()
        <<", exe: "<<handler.exe()
        <<", running: "<<handler.running()
        <<endl;


    PWNER::Cuservalue uservalue[2];
    PWNER::Ematch_type match_type;
    try {
        PWNER::string_to_uservalue(data_type, search_for, &match_type, uservalue);
    } catch (PWNER::bad_uservalue_cast &e) {
        clog<<e.what()<<endl;
        return 0;
    }

    PWNER::ByteMatches matches_first;
    timestamp = high_resolution_clock::now();
    scanner.scan_regions(matches_first, data_type, uservalue, match_type);
    clog<<"Scan 1/3 done in: "<<duration_cast<duration<double>>(high_resolution_clock::now() - timestamp).count()<<" seconds"<<endl;
    clog<<"mem_virt: "<<matches_first.mem_virt()<<endl;
    clog<<"mem_disk: "<<matches_first.mem_disk()<<endl;
    clog<<"size: "<<matches_first.size()<<endl;
    clog<<"count: "<<matches_first.count()<<endl;

    size_t limit = 10;
    clog<<"Printing first "<<limit<<" matches:"<<endl;
    for(auto v : matches_first) {
        if (limit-->0) {
            clog<<v;
            PWNER::PROCESS::Region *r = handler.get_region(v.address);
            if (r)
                clog<<", region: "<<*r;
            clog<<endl;
            continue;
        }
        clog<<"--- cut ---"<<endl;
        break;
    }

    clog<<"============================================="<<endl;

    PWNER::ByteMatches matches_prev = matches_first;
    timestamp = high_resolution_clock::now();
    scanner.scan_update(matches_prev);
    scanner.scan_recheck(matches_prev, data_type, uservalue, match_type);
    clog<<"Scan 2/3 done in: "<<duration_cast<duration<double>>(high_resolution_clock::now() - timestamp).count()<<" seconds"<<endl;
    clog<<"mem_virt: "<<matches_prev.mem_virt()<<endl;
    clog<<"mem_allo: "<<matches_prev.mem_allo()<<endl;
    clog<<"mem_disk: "<<matches_prev.mem_disk()<<endl;
    clog<<"size: "<<matches_prev.size()<<endl;
    clog<<"count: "<<matches_prev.count()<<endl;


    clog<<"============================================="<<endl;

    timestamp = high_resolution_clock::now();
    PWNER::PROCESS::ProcessHeap handler_mmap(handler);
    // PWNER::ProcessFile handler_mmap(handler, "DELMEPLZ");
    if (!handler_mmap)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");
    PWNER::ScannerByteSwath scanner_mmap(handler_mmap);


    PWNER::ByteMatches matches_curr = matches_prev;
    scanner_mmap.scan_update(matches_curr);
    scanner_mmap.scan_recheck(matches_curr, data_type, uservalue, match_type);
    clog<<"Scan 3/3 done in: "<<duration_cast<duration<double>>(high_resolution_clock::now() - timestamp).count()<<" seconds"<<endl;
    clog<<"mem_virt: "<<matches_curr.mem_virt()<<endl;
    clog<<"mem_allo: "<<matches_curr.mem_allo()<<endl;
    clog<<"mem_disk: "<<matches_curr.mem_disk()<<endl;
    clog<<"size: "<<matches_curr.size()<<endl;
    clog<<"count: "<<matches_curr.count()<<endl;

    // return 0;


    clog<<"============================================="<<endl;
    clog<<"Diff 1 3"<<endl;

    size_t c = 0;
    // size_t cc = matches_curr.size();
    size_t cc = 8500;
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
            PWNER::PROCESS::Region *r = handler.get_region((*fir).address);
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
                PWNER::PROCESS::Region *r = handler.get_region((*fir).address);
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
