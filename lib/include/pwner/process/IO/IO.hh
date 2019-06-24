//
// Created by root on 30.04.19.
//

#pragma once

#include <pwner/common.hh>
#include <pwner/process/Region.hh>
#include <cstring>  // memcpy
#include <sys/param.h> // MIN
#include <list>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class IO {
public:
    virtual ~IO() = default;
    virtual explicit operator bool() const = 0;
    virtual size_t read(uintptr_t address, void *out, size_t size) const = 0;
    virtual size_t write(uintptr_t address, void *in, size_t size) const = 0;

    virtual void update_regions() {
        for (Region& region : regions) {
            if (region.parent != 0) {
                continue;
            }
            segments.emplace_back(region);
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

    Segment *get_segment(const std::string& filename) const {
        //todo[high]: get region by path (compare also inode, device) (arguable)
        for (const Segment& region : segments)
            if (region.bin.file.filename() == filename)
                return const_cast<Segment *>(&region);
        return nullptr;
    }

    Segment *get_segment(const std::filesystem::path& filepath) const {
        //todo[high]: get region by path (compare also inode, device) (arguable)
        for (const Segment& region : segments)
            if (region.bin.file == filepath)
                return const_cast<Segment *>(&region);
        return nullptr;
    }

    constexpr static size_t npos = static_cast<size_t>(-1);

    std::vector<Region> regions;
    std::vector<Segment> segments;
};


class IOMapped : virtual public IO {
public:
    size_t read(uintptr_t address, void *out, size_t size) const override {
        Region *r = get_region(address);
        if UNLIKELY(r == nullptr)
            return npos;
        if UNLIKELY(address + size > r->address + r->size) {
            // bounds check
            size = r->size + r->address - address;
        }
        memcpy(out, reinterpret_cast<char *>(regions_mapped[(size_t)(r - &regions[0])].get() + (address - r->address)), size);
        return size;
    }

    size_t write(uintptr_t address, void *in, size_t size) const override {
        //todo[critical]: untested/to remove
        Region *r = get_region(address);
        if UNLIKELY(r == nullptr)
            return npos;
        if UNLIKELY(address + size > r->address + r->size) {
            size = r->size + r->address - address;  // bounds adjustment
        }
        memcpy(reinterpret_cast<char *>(regions_mapped[(size_t)(r - &regions[0])].get() + (address - r->address)), in, size);
        return size;
    }

public:
    std::vector<std::shared_ptr<char[]>> regions_mapped; //todo: std::vector<std::span>!
};


NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
