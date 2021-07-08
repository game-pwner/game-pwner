#pragma once

#include <utility>
#include <ostream>
#include <omp.h>
#include <pwner/common.hh>
#include <pwner/wrapper/OMP.hh>
#include <pwner/scanner/Value.hh>
#include <pwner/process/Process.hh>
#include <pwner/scanner/value/scanner_value.hh>


NAMESPACE_BEGIN(pwner)

using namespace std;
using namespace std::chrono;
// using std::cout, std::endl;
namespace sfs = std::filesystem;
namespace bio = boost::iostreams;

//
// class Offsets {
// public:
//     Offsets() = delete;
//
//     explicit Offsets(size_t level) {
//         allocated = level;
//         size = 0;
//         off = new uintptr_t[allocated];
//     }
//
//     Offsets(const Offsets& rhs) {
//         allocated = rhs.allocated;
//         size = rhs.size;
//         off = (uintptr_t *) realloc(rhs.allocated);
//         memcpy(off, rhs.off, rhs.size);
//     }
//
//     ~Offsets() {
//         delete[] off;
//     }
//
//     void push_back(uintptr_t&& o) {
//         size++;
//         if (size > allocated)
//             o
//     }
//
// public:
//     uintptr_t allocated;
//     uintptr_t size;
//     uintptr_t *off;
// };


class RegionPaths {
public:
    RegionPaths() = default;

    explicit RegionPaths(const sfs::path& file) : file(file) {};

public:
    sfs::path file;
    // std::vector<Offsets> paths;
    std::vector<std::vector<uintptr_t>> paths;
};

class RegionPaths2 {
public:
    RegionPaths2() = default;

    explicit RegionPaths2(const sfs::path& file) : file(file) {};

public:
    sfs::path file;
    // std::vector<Offsets> paths;
    std::vector<std::vector<std::pair<uintptr_t,uintptr_t>>> paths;
};


class Pointer {
public:
    /*
     * ["PwnAdventure3-Linux-Shipping"+0x034158D8] -> 0x0E9AC000
     * [0x0E9AC000+0x5F0] -> 0x0CFBE680
     * [0x0CFBE680+0x8]   -> 0x1CA66200
     * [0x1CA66200+0x250] -> 0x1EDF34A8
     * [0x1EDF34A8+0x130] -> 0x2ABBE4F0
     * 0x2ABBE4F0+0xC8    == 0x2ABBE680
     * read<float>(0x2ABBE680, &player[0].pos.z);
     */
    /*
     * [r.begin     + offset[0]] -> resolved[0]
     * [resolved[0] + offset[1]] -> resolved[1]
     * [resolved[1] + offset[2]] -> resolved[2]
     * [resolved[2] + offset[3]] -> resolved[3]
     * [resolved[3] + offset[4]] -> resolved[4]
     * resolved[4] + offset[5] == resolved[5] == resolved.back()
     * read<float>(resolved.back(), &player[0].pos.z);
     */
    Process &handler;
    sfs::path file;
    std::vector<uintptr_t> offsets;

public:
    Pointer(Process &handler, const sfs::path &file, const std::vector<uintptr_t> &offsets)
            : handler(handler), file(file), offsets(offsets) {}

    std::vector<uintptr_t>
    resolve() {
        std::vector<uintptr_t> ptr_resolved;
        ptr_resolved.reserve(offsets.size());

        uintptr_t last = handler.get_segment(file.string())->address + offsets[0];
        for (size_t i = 1; i < offsets.size(); i++) {
            if (handler.read(last, &last, sizeof(uintptr_t)) != sizeof(uintptr_t)) {
                ptr_resolved.resize(i);
                return ptr_resolved;
            }
            ptr_resolved.emplace_back(last);
            last += offsets[i];
        }
        ptr_resolved.emplace_back(last);
        return ptr_resolved;
    }
};


class scanner_pointer_backward {
public:
    explicit scanner_pointer_backward(PROCESS::IOMapped& proc)
    : proc(proc) { }

    void helper(ScannerAddress& scanner, vector<std::pair<uintptr_t,uintptr_t>> offs_addr_val, std::vector<RegionPaths2>& out) {
        std::vector<std::pair<uintptr_t,uintptr_t>> addresses;
        scanner.scan_regions(addresses, offs_addr_val.back().first - max_offset, offs_addr_val.back().first);

        offs_addr_val.emplace_back(0, 0);
        for(std::pair<uintptr_t,uintptr_t> address : addresses) {
            offs_addr_val.back() = address;
            PROCESS::Segment *sregion = proc.get_segment(address.first);
            if UNLIKELY(sregion != nullptr) {
                auto res = std::find_if(out.begin(), out.end(), [&sregion](RegionPaths2& regionPaths) { return regionPaths.file == sregion->bin.file; });
                res->paths.push_back(offs_addr_val);
            }

            if (offs_addr_val.size() <= max_level) {
                helper(scanner, offs_addr_val, out);
            }
        }
    }

    std::vector<RegionPaths2>
    scan_regions(uintptr_t address) {
        using namespace std;

        std::vector<RegionPaths2> regions;
        for(const PROCESS::Segment& sregion : proc.segments) {
            RegionPaths2 regionPaths;
            regionPaths.file = sregion.bin.file;
            regions.emplace_back(regionPaths);
        }

        ScannerAddress scanner{proc};
        scanner.step = align;

        paths_resolved = 0;


        // PROCESS::RegionStatic *sregion = proc.get_sregion(address);
        // auto res = std::find_if(regions.begin(), regions.end(), [&sregion](RegionPaths2& regionPaths) { return regionPaths.file == sregion->bin.file; });
        // res->paths.push_back(std::pair<uintptr_t,uintptr_t>{address-sregion->address, 0});

        vector<std::pair<uintptr_t,uintptr_t>> offs_addr_val;
        offs_addr_val.push_back(std::pair<uintptr_t,uintptr_t>{address, 0});
        helper(scanner, offs_addr_val, regions);

        cout<<"iterations have been done: "<<paths_resolved<<endl;
        return regions;
    }

public:
    // 3 0x800 = 0.13513 seconds
    // 4 0x800 = 5.86742 seconds
    // 5 0x800 = 197.910 seconds
    /// Create file for storing matches
    PROCESS::IOMapped& proc;
    uintptr_t max_level = 3;
    uintptr_t min_offset = 0;
    uintptr_t max_offset = 0x800;
    // uintptr_t max_offset = 0x200;
    // uintptr_t max_offset = 0x14;
    uintptr_t align = sizeof(u32);
    // uintptr_t align = 1;
    volatile double scan_progress = 0.0;
    volatile bool stop_flag = false;
    uintptr_t paths_resolved = 0;
    OMP::Lock lock;
};


class scanner_pointer_forward {
public:
    // FIXME[critical]: it must scan for multiple addresses
    explicit scanner_pointer_forward(Process& handler)
    : proc(handler) { }

    virtual ~scanner_pointer_forward() = default;

    void helper(vector<uintptr_t> off, uintptr_t last, uintptr_t address, RegionPaths& ps) {
        if (address >= last-min_offset && address < last+max_offset) {
            vector<uintptr_t> offs = off;
            offs.push_back(address-(last-min_offset));
            lock.set();
            ps.paths.push_back(offs); /// success
            lock.unset();
        }

        if (off.size() == max_level) {
            return;
        }

        off.push_back(0);
        for (size_t o1 = min_offset; o1 < max_offset; o1 += align) {
            off.back() = o1;
            uintptr_t s_last;
            if (proc.read(last+o1, &s_last, sizeof(uintptr_t)) != sizeof(uintptr_t)) {
                continue;
            }
            helper(off, s_last, address, ps);
        }
    }

    std::vector<RegionPaths>
    scan_regions(uintptr_t address) {
        using std::cout, std::endl;

        std::vector<RegionPaths> ret;

        /// for each region
        for (size_t i = 0; i < proc.segments.size(); i++) {
            PROCESS::Segment& region = proc.segments[i];
            cout<<"scan_regions: segments["<<i+1<<"/"<<proc.segments.size()<<"]: "<<region<<endl; //fixme [debug] #0
            RegionPaths ps;
            ps.file = region.bin.file.filename();

            uintptr_t static_offset_to = region.size;

            uintptr_t static_begin = region.address;
            uintptr_t static_end = region.address + static_offset_to;


            if (address >= static_begin && address < static_end) {
                std::vector<uintptr_t> off {address-static_begin};
                ps.paths.push_back(off); /// success
            }

            /// for each level0 pointer
            // #pragma omp parallel for
            for (uintptr_t o = 0; o < static_offset_to; o += align) {
                std::vector<uintptr_t> off {o};
                uintptr_t last = static_begin + o;
                if (proc.read(last, &last, sizeof(uintptr_t)) != sizeof(uintptr_t)) {
                    continue;
                }

                /// for each offset in level1
                helper(off, last, address, ps);
            }

            if (!ps.paths.empty()) {
                ret.push_back(ps);
            }
        }
        return ret;
    }

public:
    /// Create file for storing matches
    Process& proc;
    OMP::Lock lock;
    volatile bool stop_flag = false;
    volatile double scan_progress = 0.0;
    uintptr_t max_level = 3;

    uintptr_t min_offset = 0; // FIXME[critical]: not working as expected
    uintptr_t max_offset = 0x100;
    uintptr_t align = sizeof(uint32_t);

};


NAMESPACE_END(pwner)
