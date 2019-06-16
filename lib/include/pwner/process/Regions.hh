//
// Created by root on 01.05.19.
//

#pragma once

#include <bitmask/bitmask.hpp>
// #include <pwner/bin/Bin.hh>
#include <pwner/process/Region.hh>
#include <ostream>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class Regions {
public:
    Regions() = default;

    virtual ~Regions() = default;
    
    virtual void update_regions() {
        for (Region& region : regions) {
            if (region.parent != 0) {
                continue;
            }
            sregions.emplace_back(region);
        }
    }

    Region *get_region(uintptr_t address) const {
        size_t first, last, mid;
        first = 0;
        last = regions.size();
        while (first < last) {
            mid = first + (last - first) / 2; // mid = (first + last) / 2;
            if LIKELY(address < regions[mid].address) {
                last = mid;
            } else if LIKELY(address >= regions[mid].address + regions[mid].size) {
                first = mid + 1;
            } else {
                return const_cast<Region *>(&regions[mid]);
            }
        }
        return nullptr;
    }

    RegionStatic *get_sregion(uintptr_t address) const {
        size_t first, last, mid;
        first = 0;
        last = sregions.size();
        while (first < last) {
            mid = first + (last - first) / 2; // mid = (first + last) / 2;
            if LIKELY(address < sregions[mid].address) {
                last = mid;
            } else if LIKELY(address >= sregions[mid].address + sregions[mid].size) {
                first = mid + 1;
            } else {
                return const_cast<RegionStatic *>(&sregions[mid]);
            }
        }
        return nullptr;
    }

    RegionStatic *get_region(const std::string& filename) const {
        //todo[high]: get region by path (compare also inode, device) (arguable)
        for (const RegionStatic& region : sregions)
            if (region.bin.file.filename() == filename)
                return const_cast<RegionStatic *>(&region);
        return nullptr;
    }

    RegionStatic *get_region(const std::filesystem::path& filepath) const {
        //todo[high]: get region by path (compare also inode, device) (arguable)
        for (const RegionStatic& region : sregions)
            if (region.bin.file == filepath)
                return const_cast<RegionStatic *>(&region);
        return nullptr;
    }

public:
    std::vector<Region> regions;
    std::vector<RegionStatic> sregions;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
