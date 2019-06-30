//
// Created by root on 23.03.19.
//

#include <iostream>
#include <string>
#include <chrono>
#include <pwner/process/Process.hh>
#include <pwner/scanner/value/scanner_value.hh>
#include <pwner/scanner/pointer/scanner_pointer.hh>
#include <pwner/scanner/Value.hh>


int main(int argc, const char *argv[]) {
    using std::cout, std::clog, std::cerr, std::endl;
    using namespace pwner;
    oom_score_adj(801);

    // std::string target = "PwnAdventure3-Linux-Shipping";
    std::string target = "FAKEMEM";
    ProcessProcfs proc{target};
    if (!proc)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");

    clog<<target
        <<", pid: "<<proc.pid()
        <<", cmdline: "<< proc.cmdline()
        <<", running: "<<proc.running()
        <<endl;

    scanner_pointer_forward ps{proc};

    PROCESS::Segment *segment = proc.get_segment(target);
    if (!segment)
        throw std::runtime_error("Cannot find segment "+target);
    uintptr_t needle = segment->address+0x10;
    cout<<"needle: "<<HEX(needle)<<endl;

    std::vector<RegionPaths> scan_result;
    {
        Timer t{"scaning"};
        scan_result = ps.scan_regions(needle); //fakemem`begin+0x10 //43 pointera
    }
    // *(*("FAKEMEM"+302100)+4)+40
    // "FAKEMEM"+302100->+4->+40
    cout<<"pointer list:"<<endl;
    for (const auto& region : scan_result) {
        for(const std::vector<uintptr_t>& offsets : region.paths) {
            cout<<region.file.filename()<<"+";
            for(const uintptr_t& o : offsets) {
                cout<<HEX(o)<<"+";
            }
            cout<<"\b"<<endl;
        }
    }

    cout<<"No error"<<endl;
    return 0;
}
