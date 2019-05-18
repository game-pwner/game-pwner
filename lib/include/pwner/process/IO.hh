//
// Created by root on 30.04.19.
//

#pragma once

#include <sys/param.h>
#include <pwner/common.hh>
#include <pwner/process/Regions.hh>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class IO : public Regions {
public:
    IO() = default;
    virtual ~IO() = default;
    virtual explicit operator bool() const = 0;
    virtual size_t read(uintptr_t address, void *out, size_t size) const = 0;
    virtual size_t write(uintptr_t address, void *in, size_t size) const = 0;

    template<typename T>
    bool read(uintptr_t address, T *out) const { return read(address, out, sizeof(T)) == sizeof(T); }

    template<typename T>
    bool write(uintptr_t address, T *in) const { return write(address, in, sizeof(T)) == sizeof(T); }

public:
    /// Value returned by various member functions when they fail.
    static constexpr size_t npos = static_cast<size_t>(-1);
};


class IOMapped : virtual public IO {
public:
    size_t read(uintptr_t address, void *out, size_t size) const override {
        https://www.google.com
        Region *r = get_region(address);
        if UNLIKELY(r == nullptr)
            return npos;
        if UNLIKELY(address + size > r->address + r->size) {
            size = r->size + r->address - address;
        }
        memcpy(out, reinterpret_cast<char *>(regions_mapped[r - &regions[0]] + (address - r->address)), size);
        return size;
    }

    size_t write(uintptr_t address, void *in, size_t size) const override {
        //todo[critical]: untested/to remove
        Region *r = get_region(address);
        if UNLIKELY(r == nullptr)
            return npos;
        if UNLIKELY(address + size > r->address + r->size) {
            size = r->size + r->address - address;
        }
        memcpy(reinterpret_cast<char *>(regions_mapped[r - &regions[0]] + (address - r->address)), in, size);
        return size;
    }

public:
    std::vector<char *> regions_mapped;
};


class IOMappedHeap : public IOMapped {
public:
    explicit IOMappedHeap(IO &io) {
        regions = io.regions;
        sregions = io.sregions;

        regions_mapped.clear();
        for (const Region& region : regions) {
            char *map = new char[region.size];
            regions_mapped.emplace_back(map);

            // FIXME[med]: bug: can not copy some regions.
            size_t copied = io.read(region.address, map, region.size);
            if (copied == npos) {
                memset(map, 0, region.size);
                std::clog<<"Cannot read region: "<<region<<std::endl;
            }
        }
    };
    explicit operator bool() const override {
        return !regions_mapped.empty();
    };
};


// class IOMappedFile : public IOMapped {
// public:
//     explicit IOMappedFile(const IOMapped &io);
//     explicit IOMappedFile(const std::filesystem::path &file);
//     explicit operator bool() const override;
//     bool open(const std::filesystem::path &file);
//     bool save(const std::filesystem::path &file);
//
// protected:
//     std::string magic = "PWNER";
// };





// ┌──────────────────────────────────────────────────────────────────┐
//   class PWNER::IOMappedFile;
// └──────────────────────────────────────────────────────────────────┘
// IOMappedFile::IOMappedFile(const IOMapped &io) {
// TODO[med]: criu
// }

// IOMappedFile::IOMappedFile(const std::filesystem::path &path) {
//     TODO[med]: criu
//     std::ifstream stream(path, std::ios_base::in | std::ios_base::binary);
//     std::string m(magic);
//     stream.read(&m[0], m.size());
//
//     if (!stream || m != magic) {
//         return;
//     }
//     boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
//     archive >> *this;
//
//     params.path = path;
//     params.mode = bio::mapped_file::mapmode::readwrite;
//     mf.open(params);
//     if (!mf.is_open())
//         throw std::invalid_argument("can not open '" + path + "'");
//
//     char* snapshot = mf.data();
//     assert(stream.is_open());
//     assert(stream.tellg() != -1);
//     snapshot += stream.tellg();
//
//     regions_on_map.clear();
//     for (const RE::Region& region : regions) {
//         regions_on_map.emplace_back(snapshot);
//         //snapshot += region.size + sizeof(mem64_t::bytes) - 1;
//         snapshot += region.size;
//     }
// }
//
// IOMappedFile::operator bool() const {
//     return false;
// }
//
// bool IOMappedFile::open(const std::filesystem::path &file) {
//     return false;
// }
//
// bool IOMappedFile::save(const std::filesystem::path &file) {
//     return false;
// }


NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
