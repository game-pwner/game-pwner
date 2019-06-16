//
// Created by root on 30.04.19.
//

#pragma once

#include <pwner/common.hh>
#include <pwner/process/Regions.hh>
#include <cstring>  // memcpy
#include <sys/param.h> // MIN


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class IO : virtual public Regions {
public:
    virtual explicit operator bool() const = 0;
    virtual size_t read(uintptr_t address, void *out, size_t size) const = 0;
    virtual size_t write(uintptr_t address, void *in, size_t size) const = 0;

public:
    /// Value returned by various member functions when they fail.
    static constexpr size_t npos = static_cast<size_t>(-1);
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
        // memcpy(out, reinterpret_cast<char *>(regions_mapped[static_cast<size_t>(r - &regions[0])][(static_cast<ptrdiff_t>(address - r->address))]), size);
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
        memcpy(reinterpret_cast<char *>(regions_mapped[(size_t)(r - &regions[0])].get() + (address - r->address)), in, size);
        return size;
    }

public:
    std::vector<std::shared_ptr<char[]>> regions_mapped;
};


/** The cached reader made for reading small values many times to reduce system calls */
class IOCached : public IO {
public:
    explicit IOCached(const IO& parent)
    : m_io(parent), m_base(0), m_cache_size(0) {
        m_cache.resize(MAX_PEEKBUF_SIZE);
    }

    void reset() const {
        m_base = 0;
        m_cache_size = 0;
    }

    size_t read(uintptr_t address, void *out, size_t size) const override {
        if UNLIKELY(size > MAX_PEEKBUF_SIZE) {
            return m_io.read(address, out, size);
        }

        if LIKELY(m_base
                  && address >= m_base
                  && address - m_base + size <= m_cache_size) {
            /* full cache hit */
            memcpy(out, &m_cache[address - m_base], size);
            return size;
        }

        /* we need to retrieve memory to complete the request */
        size_t len = m_io.read(address, &m_cache[0], MAX_PEEKBUF_SIZE);
        if UNLIKELY(len == IO::npos) {
            /* hard failure to retrieve memory */
            reset();
            return IO::npos;
        }

        m_base = address;
        m_cache_size = len;

        /* return result to caller */
        memcpy(out, &m_cache[0], size);
        return MIN(size, m_cache_size);
    }

    size_t write(uintptr_t address, void *in, size_t size) const override {
        return m_io.write(address, in, size);
    }

    explicit operator bool() const override {
        return m_io.operator bool();
    }

public:
    static constexpr size_t MAX_PEEKBUF_SIZE = 4u << 10u;

private:
    const IO& m_io;
    mutable uintptr_t m_base; // base address of cached region
    mutable size_t m_cache_size;
    mutable std::vector<uint8_t> m_cache;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
