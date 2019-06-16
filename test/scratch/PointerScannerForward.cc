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

    clog<<target<<", pid: "
        <<handler.pid()
        <<", cmdline: "<< handler.cmdline()
        <<", running: "<<handler.running()
        <<endl;
    clog.flush();

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

    // PWNER::PointerScannerForward ps(&handler);
    //or
    // PWNER::ProcessF processF(handler, "asd");
    // PWNER::PointerScannerForward ps(&processF);
    //or
    PWNER::PROCESS::ProcessHeap processH(handler);
    // PWNER::PointerScannerForward ps(processH);
    PWNER::PointerScannerForward ps(processH);

    PWNER::PROCESS::RegionStatic *region = handler.get_region(target);
    if (!region)
        throw std::runtime_error("Cannot find region "+target);
    uintptr_t seek = region->address+0x10;
    cout<<"seek: "<<HEX(seek)<<endl;

    timestamp = high_resolution_clock::now();
    // std::vector<PWNER::RegionPaths> scan_result = ps.scan_regions(0x7f50dc812000);
    std::vector<PWNER::RegionPaths> scan_result = ps.scan_regions(seek); //fakemem`begin+0x10 //43 pointera
    // std::vector<PWNER::RegionPaths> scan_result = ps.scan_regions(0x7FFCA82F56C0); //fakemem's bytes allocated
    clog<<"PointerScanForward done in: "<<duration_cast<duration<double>>(high_resolution_clock::now() - timestamp).count()<<" seconds"<<endl;

    // *(*("FAKEMEM"+302100)+4)+40
    // "FAKEMEM"+302100->+4->+40
    cout<<"pointer list:"<<endl;
    for (const PWNER::RegionPaths& region : scan_result) {
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
