/*
    This file is part of Reverse Engine.

    Array of scanner results.

    Copyright (C) 2017-2018 Ivan Stepanov <ivanstepanovftw@gmail.com>
    Copyright (C) 2015,2017 Sebastian Parschauer <s.parschauer@gmx.de>
    Copyright (C) 2017      Andrea Stacchiotti <andreastacchiotti@gmail.com>
    Copyright (C) 2010      WANG Lu <coolwanglu@gmail.com>
    Copyright (C) 2009      Eli Dupree <elidupree@harter.net>

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <pwner/common.hh>
#include <pwner/process/IO/IO.hh>


NAMESPACE_BEGIN(pwner)

/** The cached reader made for reading small values many times to reduce system calls */
class IOCached {
public:
    explicit IOCached(const PROCESS::IO& parent)
    : m_io(parent), m_base(0), m_cache_size(0) {
        m_cache.resize(MAX_PEEKBUF_SIZE);
    }

    void reset() const {
        m_base = 0;
        m_cache_size = 0;
    }

    size_t read(uintptr_t address, void *out, size_t size) const {
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
        if UNLIKELY(len == PROCESS::IO::npos) {
            /* hard failure to retrieve memory */
            reset();
            return PROCESS::IO::npos;
        }

        m_base = address;
        m_cache_size = len;

        /* return result to caller */
        memcpy(out, &m_cache[0], size);
        return MIN(size, m_cache_size);
    }

    size_t read(uintptr_t address, void **out) const {
        size_t len = m_io.read(address, &m_cache[0], MAX_PEEKBUF_SIZE);
        if UNLIKELY(len == PROCESS::IO::npos) {
            /* hard failure to retrieve memory */
            reset();
            return PROCESS::IO::npos;
        }

        m_base = address;
        m_cache_size = len;

        /* return result to caller */
        *out = reinterpret_cast<void*>(&m_cache[0]);
        return m_cache_size;
    }

    size_t write(uintptr_t address, void *in, size_t size) const {
        return m_io.write(address, in, size);
    }

public:
    static constexpr size_t MAX_PEEKBUF_SIZE = 4u << 10u;

private:
    const PROCESS::IO& m_io;
    mutable uintptr_t m_base; // base address of cached region
    mutable size_t m_cache_size;
    mutable std::vector<uint8_t> m_cache;
};


// class MemoryIterator {
// public:
//     explicit MemoryIterator(const PROCESS::IO& proc, uintptr_t begin, uintptr_t end)
//     : m_proc{proc}, m_begin{begin}, m_end{end} {}
//
//     class iterator : public std::iterator<
//             std::forward_iterator_tag, // iterator_category
//             long,                      // value_type
//             long,                      // difference_type
//             const long*,               // pointer
//             const long&                // reference
//     >{
//         long num = FROM;
//     public:
//         iterator(long _num = 0) : num(_num) {}
//         iterator& operator++() {num = TO >= FROM ? num + 1: num - 1; return *this;}
//         iterator operator++(int) {iterator retval = *this; ++(*this); return retval;}
//         bool operator==(iterator other) const {return num == other.num;}
//         bool operator!=(iterator other) const {return !(*this == other);}
//         long operator*() {return num;}
//     };
//     iterator begin() {return FROM;}
//     iterator end() {return TO >= FROM? TO+1 : TO-1;}
//
// public:
//     /// Create file for storing matches
//     const PROCESS::IO& m_proc;
//     const uintptr_t m_begin;
//     const uintptr_t m_end;
// };

NAMESPACE_END(pwner)
