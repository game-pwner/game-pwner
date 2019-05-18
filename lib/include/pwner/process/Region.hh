//
// Created by root on 10.05.19.
//

#pragma once

#include <pwner/bin/standart/ELF.hh>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

enum class region_mode_t : uint8_t {
    none = 0u,
    executable = 1u<<0u,
    writable   = 1u<<1u,
    readable   = 1u<<2u,
    shared     = 1u<<3u,
};
BITMASK_DEFINE_VALUE_MASK(region_mode_t, 0xff)


class Region {
public:
    Region()
    : id(npos), parent(npos), address(0), size(0), mode(region_mode_t::none), offset(0),
      st_device_minor(0), st_device_major(0), inode(0) { }

    virtual ~Region() = default;

    explicit constexpr operator bool() const {
        return address != 0;
    }

    std::string str() const {
        std::ostringstream ss;
        ss << HEX(id) << " (" << HEX(parent) << ") "
           << HEX(address)<<"-"<<HEX(address+size)<<"="<<HEX(size)<<" "
           << (mode & region_mode_t::shared?"s":"p") << (mode & region_mode_t::readable?"r":"-") << (mode & region_mode_t::writable?"w":"-") << (mode & region_mode_t::executable?"x":"-") << " "
           << HEX(offset) << " "
           << HEX(st_device_major)<<":"<< HEX(st_device_minor) <<" "
           << inode<<" ";
        ss << file;
        return ss.str();
    }

// private:
//     friend class boost::serialization::access;
//
//     template<class Archive>
//     void serialize(Archive& ar, const unsigned int version) {
//         ar & address;
//         ar & size;
//         ar & *reinterpret_cast<decltype(mode)::underlying_type*>(&mode);
//         ar & *reinterpret_cast<decltype(mode)::underlying_type*>(&mode);
//         ar & offset;
//         ar & st_device_minor;
//         ar & st_device_major;
//         ar & inode;
//         ar & file;
//     }

    friend std::ostream &operator<<(std::ostream &os, const Region &region) {
        return os << region.str();
    }

public:
    size_t id;
    size_t parent;
    uintptr_t address;
    uintptr_t size;
    bitmask::bitmask<region_mode_t> mode;

    /// File data
    uintptr_t offset;
    uint8_t st_device_minor;  /* Ambiguous todo rename */
    uint8_t st_device_major;  /* Ambiguous */
    uint64_t inode;
    sfs::path file;

    /// Value returned by various member functions when they fail.
    static constexpr size_t npos = static_cast<size_t>(-1);
};


class RegionStatic {
public:
    explicit RegionStatic(Region& region)
    : id(region.id), bin(region.file), address(region.address), size(0) {
        for(auto &section : bin.get_sections()) {
            uintptr_t __sz = section.section_addr + section.section_size;
            size = std::max(size, __sz);
        }
    }

    virtual ~RegionStatic() = default;

    std::string str() const {
        std::ostringstream ss;
        ss << HEX(address)<<"-"<<HEX(address+size)<<"="<<HEX(size)<<" "
           << bin.file.filename();
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& outputStream, const RegionStatic& sregion) {
        return outputStream<<sregion.str();
    }

public:
    size_t id;
    BIN::STANDART::ELF bin;
    uintptr_t address;
    uintptr_t size;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
