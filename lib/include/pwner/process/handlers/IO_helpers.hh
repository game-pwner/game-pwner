//
// Created by root on 10.05.19.
//

#pragma once

#include <pwner/process/IO.hh>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

/** The cached reader made for reading small values many times to reduce system calls */
template <class __IO, class = typename std::enable_if<std::is_base_of<PROCESS::IO,  __IO>::value>::type>
class IOCached {
public:
    explicit IOCached(__IO& parent)
            : m_parent(parent) {
        m_base = 0;
        m_cache_size = 0;
        m_cache.resize(MAX_PEEKBUF_SIZE);
    }

    virtual ~IOCached() = default;

    [[gnu::always_inline]]
    inline void reset() const {
        m_base = 0;
        m_cache_size = 0;
    }

    // template<class PH = __IO, typename std::enable_if<std::is_base_of<PH, ProcessF>::value>::type* = nullptr>
    // [[gnu::always_inline]] inline
    size_t read(uintptr_t address, void *out, size_t size) const {
        if UNLIKELY(size > MAX_PEEKBUF_SIZE) {
            return m_parent.read(address, out, size);
        }

        if LIKELY(m_base
                  && address >= m_base
                  && address - m_base + size <= m_cache_size) {
            /* full cache hit */
            memcpy(out, &m_cache[address - m_base], size);
            return size;
        }

        /* we need to retrieve memory to complete the request */
        size_t len = m_parent.read(address, &m_cache[0], MAX_PEEKBUF_SIZE);
        if UNLIKELY(len == m_parent.npos) {
            /* hard failure to retrieve memory */
            reset();
            return m_parent.npos;
        }

        m_base = address;
        m_cache_size = len;

        /* return result to caller */
        memcpy(out, &m_cache[0], size);
        return MIN(size, m_cache_size);
    }

    // template<class PH = __IO, typename std::enable_if<!std::is_base_of<PH, ProcessF>::value>::type* = nullptr>
    // [[gnu::always_inline]] inline
    // size_t read(uintptr_t address, void *out, size_t size) {
    //     return m_parent.read(address, out, size);
    // }

public:
    static constexpr size_t MAX_PEEKBUF_SIZE = 4u * (1u << 10u); // 4 KiB

private:
    __IO& m_parent;
    mutable uintptr_t m_base; // base address of cached region
    mutable size_t m_cache_size;
    mutable std::vector<uint8_t> m_cache;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
