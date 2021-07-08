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

    ProcessHeap proc_f{proc};
    
    scanner_pointer_backward ps{proc_f};

    PROCESS::Segment *segment = proc.get_segment(target);
    if (!segment)
        throw std::runtime_error("Cannot find segment "+target);
    uintptr_t needle = segment->address+0x10;
    cout<<"needle: "<<HEX(needle)<<endl;

    std::vector<RegionPaths2> scan_result;
    {
        Timer t{"scaning"};
        scan_result = ps.scan_regions(needle); //fakemem`begin+0x10 //43 pointera
    }
    // *(*("FAKEMEM"+302100)+4)+40
    // "FAKEMEM"+302100->+4->+40
    size_t count = 0;
    cout<<"pointer list (+1):"<<endl;
    for (const RegionPaths2& region : scan_result) {
        for(const std::vector<std::pair<uintptr_t,uintptr_t>>& offsets : region.paths) {
            count++;
            cout<<region.file.filename()<<"+"<<HEX(offsets[offsets.size()-1].first - proc_f.get_segment(region.file)->address)<<"+";
            for(size_t i = offsets.size() - 1; i >= 1; i--) {
                cout<<HEX((offsets[i-1].first - offsets[i].second))<<"+";
            }
            cout<<"\b"<<endl;
        }
    }
    cout<<"count: "<<count<<endl;
    // cout << "found lvl.3: "<<sregion_3->bin.file.filename()<<"+"<<HEX(address_3.first - sregion_3->address)
    //       << "+"<<HEX(address_2.first - address_3.second)
    //       << "+"<<HEX(address_1.first - address_2.second)
    //       << "+"<<HEX(address_0       - address_1.second)<<endl;

    cout<<"No error"<<endl;
    return 0;
}
