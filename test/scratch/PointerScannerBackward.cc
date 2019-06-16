//
// Created by root on 23.03.19.
//

#include <iostream>
#include <string>
#include <chrono>
#include <pwner/process/Process.hh>
#include <pwner/scanner/ValueScanner.hh>
#include <pwner/scanner/PointerScanner.hh>
#include <pwner/scanner/Value.hh>


int main(int argc, const char *argv[]) {
    using std::cout, std::clog, std::cerr, std::endl;
    using namespace std::chrono;
    oom_score_adj(801);

    high_resolution_clock::time_point timestamp;

    // std::string target = "PwnAdventure3-Linux-Shipping";
    std::string target = "FAKEMEM";

    PWNER::PROCESS::ProcessProcfs handler(target);
    if (!handler)
        throw std::invalid_argument("Cannot find "+target+" process. Nothing to do.");


    clog<<target
        <<", pid: "<<handler.pid()
        <<", cmdline: "<< handler.cmdline()
        <<", exe: "<<handler.exe()
        <<", running: "<<handler.running()
        <<endl;

    // ----------------------------------------------------------
    // std::string pointer_region = "libtier0_client.so";
    // std::vector<uintptr_t> pointer_offset {
    //     0x6cc0, // 5f32 2e33 2e34 // "_2.3.4"
    // };
    // cout<<"pos_z: "<<*reinterpret_cast<float*>(&value)<<endl;
    // ----------------------------------------------------------
    // std::string pointer_region = "PwnAdventure3-Linux-Shipping";
    // std::vector<ptrdiff_t> pointer_offset {
    //         0x034158D8,
    //         0x5F0,
    //         0x8,
    //         0x250,
    //         0x130,
    //         0xC8,
    // };
    // cout<<"pos_z: "<<value<<endl;
    // ----------------------------------------------------------
    //std::string pointer_region = "FAKEMEM";
    // ----------------------------------------------------------

    // PWNER::Pointer p(handler, pointer_region, pointer_offset);

    // std::vector<uintptr_t> ptr_resolved = p.resolve();
    // if (pointer_offset.size() != ptr_resolved.size()) {
    //     cout<<"Pointer is not fully resolved: "<<pointer_offset.size() <<" != "<< ptr_resolved.size()<<endl;
    // } else {
    //     uint64_t value = 0;
    //     if (!handler.read(ptr_resolved.back(), &value)) {
    //         exit(1);
    //     }
    //     cout<<"pos_z: "<<reinterpret_cast<char*>(&value)<<endl;
    // }
    //
    // cout<<"ptr_resolved.back(): "<<PWNER::HEX(ptr_resolved.back())<<endl;

    // PWNER::PointerScannerBackward ps(&handler);
    //or
    // PWNER::ProcessF processF(handler, "asd");
    // PWNER::PointerScannerBackward ps(&processF);
    //or
    PWNER::PROCESS::ProcessHeap processH(handler);
    // PWNER::PointerScannerBackward ps(processH);
    PWNER::PointerScannerBackward ps(processH);

    PWNER::PROCESS::RegionStatic *r = processH.get_region(target);
    if (!r)
        throw std::invalid_argument("Cannot find "+target+" region. Nothing to do.");
    uintptr_t seek = r->address+0x10;
    cout<<"seek: "<<HEX(seek)<<endl;

    timestamp = high_resolution_clock::now();
    std::vector<PWNER::RegionPaths2> scan_result = ps.scan_regions(seek);
    clog<<"PointerScanBackward done in: "<<duration_cast<duration<double>>(high_resolution_clock::now() - timestamp).count()<<" seconds"<<endl;


    sleep(5);
    size_t count = 0;
    cout<<"pointer list (+1):"<<endl;
    for (const PWNER::RegionPaths2& region : scan_result) {
        for(const std::vector<std::pair<uintptr_t,uintptr_t>>& offsets : region.paths) {
            count++;
            cout<<region.file.filename()<<"+"<<HEX(offsets[offsets.size()-1].first - processH.get_region(region.file)->address)<<"+";
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
