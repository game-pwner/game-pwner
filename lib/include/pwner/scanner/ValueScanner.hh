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

#include <cinttypes>
#include <memory>
#include <cmath>
#include <chrono>
#include <fcntl.h>
#include <sys/mman.h>
#include <chrono>
#include <deque>
#include <string>
#include <cstdlib>
#include <pwner/process/Process.hh>
#include <pwner/process/IO/IO.hh>
#include <pwner/scanner/Value.hh>
#include "cinttypes"
#include "memory"
#include "cmath"
#include "chrono"
#include "fcntl.h"
#include "sys/mman.h"
#include "pwner/process/Process.hh"
#include "Value.hh"


NAMESPACE_BEGIN(PWNER)

typedef unsigned int (*scan_routine_t)(const PWNER::mem64_t *memory_ptr,
                                       size_t memlength,
                                       const PWNER::value_t *old_value,
                                       const PWNER::Cuservalue *user_value,
                                       PWNER::flag& saveflags);
extern scan_routine_t sm_scan_routine;

/// 1
// typedef void (&MyFunc)(int,int);
// using MyFunc = void(int,int);
/// 2
// typedef void MyFunc(int,int);
// using MyFunc = void(&)(int,int);



/*
* Choose the global scanroutine according to the given parameters, sm_scan_routine will be set.
* Returns whether a proper routine has been found.
*/
bool sm_choose_scanroutine(PWNER::Edata_type dt,
                           PWNER::Ematch_type mt,
                           const PWNER::Cuservalue* uval,
                           bool reverse_endianness);

scan_routine_t sm_get_scanroutine(PWNER::Edata_type dt,
                                  PWNER::Ematch_type mt,
                                  const flag& uflags,
                                  bool reverse_endianness);


/**
 *  Better to have multiple little arrays called 'swath_t', than one large array of bytes and flags.
 *  E.g. target program have array of uint32_t full of zeros and ones:
 *      00010010100000100000000000000000010010111111000
 *  and we wanted to match only '1':
 *      00010010100000100000000000000000010010111111000
 *         ^~ ^~~~    ^~                 ^~~~~~~~~~~~
 *  Using 'swath_t' we will use less memory space.
 */
class ByteSwath {
public:
    explicit ByteSwath(uintptr_t base_address) {
        this->base_address = base_address;
    }

    void append(uint8_t byte, const flag& flag) {
        bytes.emplace_back(byte);
        flags.emplace_back(flag);
        // if UNLIKELY(flags.size() != bytes.size()) {
        //     throw runtime_error("flags.size() != bytes.size(): "+to_string(flags.size())+" != "+to_string(bytes.size()));
        // }
    }

    uintptr_t remote_get(size_t n) {
        return base_address + static_cast<uintptr_t>(n);
    }

    uintptr_t remote_back() {
        return remote_get(bytes.size() - 1);
    }

    // only at most sizeof(int64_t) bytes will be read,
    // if more bytes are needed (e.g. bytearray),
    // read them separately (for performance)
    value_t to_value(size_t index) {
        //todo[high]: do constructor like value_t(ByteSwath)
        value_t val;

        val.address = remote_get(index);

        /* Truncate to the old flags, which are stored with the first matched byte */
        // if UNLIKELY(flags.size() != bytes.size()) {
        //     throw runtime_error("flags.size() != bytes.size(): "+to_string(flags.size())+" != "+to_string(bytes.size()));
        // }
        // if UNLIKELY(index >= flags.size()) {
        //     throw runtime_error("index >= flags.size(): "+to_string(index)+" >= "+to_string(flags.size()));
        // }
        val.flags = flags[index];

        size_t max_bytes = bytes.size() - index;

        /* NOTE: This does the right thing for VLT because the flags are in
         * the same order as the number representation (for both endians), so
         * that the zeroing of a flag does not change useful bits of `length`. */
        if (max_bytes > 8)
            max_bytes = 8;

        memcpy(val.bytes, &bytes[index], max_bytes);

        return val;
    }

public:
    uintptr_t base_address;
    std::vector<uint8_t> bytes;
    std::vector<flag> flags;
};


/**
 *  Master array of matches array.
 */
class ByteMatches {
public:
    //left side
    //std::ostream file;
    //right side
    std::vector<ByteSwath> swaths;

    ByteMatches() { }

    ~ByteMatches() { }

    [[gnu::always_inline]]
    void
    add_element(const uintptr_t& remote_address, const mem64_t *remote_memory, const flag& flags)
    {
        if UNLIKELY(remote_address == 0)
            throw std::bad_exception();
        if UNLIKELY(swaths.empty()) {
            swaths.emplace_back(remote_address);
        }

        constexpr size_t preallocated_bytes = 1;
        constexpr size_t size_of_bytes_flags = preallocated_bytes*(sizeof(decltype(ByteSwath::bytes)::value_type) + sizeof(decltype(ByteSwath::flags)::value_type));
        size_t remote_delta = remote_address - swaths.back().remote_back();
        size_t local_delta = remote_delta * size_of_bytes_flags;

        if (local_delta >= sizeof(ByteSwath) + size_of_bytes_flags) {
            /* It is more memory-efficient to start a new swath */
            swaths.emplace_back(remote_address);
        } else {
            /* It is more memory-efficient to write over the intervening space with null values */
            while (remote_delta-- > 1)
                swaths.back().append(0, PWNER::flag(PWNER::flag_t::flags_empty));
        }
        swaths.back().append(remote_memory->u8, flags);
        assert(swaths.back().flags.size() == swaths.back().bytes.size());
    }

    size_t mem_allo() {
        size_t size = 0;
        for(auto& swath : swaths)
            size += swath.bytes.capacity() * sizeof(decltype(swath.bytes)::value_type)
                    + swath.flags.capacity() * sizeof(decltype(swath.flags)::value_type);
        return size + swaths.capacity() * sizeof(ByteSwath);
    }

    size_t mem_virt() {
        size_t size = 0;
        for(auto& swath : swaths)
            size += swath.bytes.size() * sizeof(decltype(swath.bytes)::value_type)
                    + swath.flags.size() * sizeof(decltype(swath.flags)::value_type);
        return size + swaths.size() * sizeof(ByteSwath);
    }

    size_t mem_disk() {
        size_t size = 0;
        for(auto& swath : swaths)
            size += swath.bytes.size() * sizeof(decltype(swath.bytes)::value_type)
                    + swath.flags.size() * sizeof(decltype(swath.flags)::value_type);
        return size + swaths.size() * sizeof(ByteSwath);
    }

    size_t size() {
        size_t matches = 0;
        for(auto& swath : swaths)
            for(auto& f : swath.flags)
                matches++;
        return matches;
    }

    size_t count() {
        size_t valid_matches = 0;
        for(auto& swath : swaths)
            for(auto& f : swath.flags)
                if (f != flag_t::flags_empty)
                    valid_matches++;
        return valid_matches;
    }


    struct iterator {
        typedef value_t value_type;
        typedef value_t& reference_type;
        typedef value_t* pointer_type;
        typedef value_t difference_type;
        typedef std::output_iterator_tag iterator_category;

        iterator& operator++() {
            using namespace std;
            value_index++;
            for(; swath_index < parent->swaths.size(); swath_index++, value_index = 0) {
                for(; value_index < parent->swaths[swath_index].flags.size(); value_index++) {
                    if (parent->swaths[swath_index].flags[value_index] != PWNER::flag_t::flags_empty) {
                        // clog<<"++: found: swath_index: "<<swath_index<<" (size "<<parent->swaths.size()<<")"<<endl;
                        // if (swath_index < parent->swaths.size())
                        //     clog<<"++: found:     value_index: "<<value_index<<" (size "<<parent->swaths[swath_index].bytes.size()<<")"<<endl;
                        return *this;
                    }
                    // clog<<"++: next: swath_index: "<<swath_index<<" (size "<<parent->swaths.size()<<")"<<endl;
                    // if (swath_index < parent->swaths.size())
                    //     clog<<"++: next:     value_index: "<<value_index<<" (size "<<parent->swaths[swath_index].bytes.size()<<")"<<endl;
                }
            }
            // clog<<"++: end: swath_index: "<<swath_index<<" (size "<<parent->swaths.size()<<")"<<endl;
            // if (swath_index < parent->swaths.size())
            //     clog<<"++: end:     value_index: "<<value_index<<" (size "<<parent->swaths[swath_index].bytes.size()<<")"<<endl;
            swath_index = parent->swaths.size();
            value_index = 0;
            return *this;
        }

        const iterator operator++(int) const {
            //fixme[critical]: c++ not working
            iterator it(parent, swath_index, value_index);
            ++it;
            return it;
        }

        value_type operator*() const {
            // clog<<"*:     (size "<<parent->swaths.size()<<")"<<endl;
            // if (swath_index < parent->swaths.size())
            //     clog<<"dereference:     value_index: "<<value_index<<" (size "<<parent->swaths[swath_index].bytes.size()<<")"<<endl;
            if (swath_index >= parent->swaths.size() || value_index >= parent->swaths[swath_index].bytes.size()) {
                throw std::range_error("ByteMatches: dereference outside of range");
            }
            return parent->swaths[swath_index].to_value(value_index);
        }

        friend void swap(iterator& lhs, iterator& rhs) {
            std::swap(lhs.parent, rhs.parent);
            std::swap(lhs.swath_index, rhs.swath_index);
            std::swap(lhs.value_index, rhs.value_index);
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs) {
            return lhs.parent==rhs.parent && lhs.swath_index==rhs.swath_index && lhs.value_index==rhs.value_index;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs) {
            bool result = !operator==(lhs, rhs);
            // clog<<"!=: will return "<<result<<endl;
            return result;
        }
    private:
        friend ByteMatches;
        ByteMatches *parent;
        size_t swath_index;
        size_t value_index;

        explicit iterator(ByteMatches* container, size_t swath = 0, size_t value = 0) : parent(container), swath_index(swath), value_index(value) {
            for (; swath_index < parent->swaths.size(); swath_index++) {
                for (size_t d = 0; d < parent->swaths[swath_index].flags.size(); d++) {
                    if (parent->swaths[swath_index].flags[d] == PWNER::flag_t::flags_empty)
                        continue;
                    value_index = d;
                    // clog<<"begin iterator: swath_index: "<<swath_index<<" (size "<<parent->swaths.size()<<")"<<endl;
                    // if (swath_index < parent->swaths.size())
                    //     clog<<"begin iterator:    value_index: "<<value_index<<" (size "<<parent->swaths[swath_index].bytes.size()<<")"<<endl;
                    return;
                }
            }
            // clog<<"end iterator: swath_index: "<<swath_index<<" (size "<<parent->swaths.size()<<")"<<endl;
            // if (swath_index < parent->swaths.size())
            //     clog<<"end iterator:    value_index: "<<value_index<<" (size "<<parent->swaths[swath_index].bytes.size()<<")"<<endl;
        }
    };

    iterator begin() { return iterator(this); }

    iterator end() { return iterator(this, this->swaths.size()); }

    const value_t operator[](size_t key) {
        size_t i = 0;
        for (size_t s = 0; s < swaths.size(); s++) {
            for (size_t d = 0; d < swaths[s].flags.size(); d++) {
                if (swaths[s].flags[d] == PWNER::flag_t::flags_empty)
                    /* only actual matches are considered */
                    continue;
                if (i == key)
                    /* that's it! */
                    return this->swaths[s].to_value(d);
                i++;
            }
        }
        throw std::out_of_range("");
    }

    //const value_t operator[](size_t key) const { return this[key]; }
};


class ScannerByteSwath {
public:
    explicit ScannerByteSwath(std::shared_ptr<PROCESS::Process>& handler)
    : handler(std::move(handler)) {}

    virtual ~ScannerByteSwath() = default;

    /**
     *  This is used as first scan. It scans for values and allocates memory for each match.
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_regions(PWNER::ByteMatches& writing_matches,
                      const PWNER::Edata_type& data_type,
                      const PWNER::Cuservalue *uservalue,
                      const PWNER::Ematch_type& match_type);

    /**
     *  Update bytes of matches (it does not scan, it just updates value).
     *  Should be used after scan_regions().
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_update(PWNER::ByteMatches& writing_matches);

    /**
     *  Remove flag for each byte if value mismatched.
     *  Should be used after scan_update().
     *
     *  Will update 'matches_t'
     */
    bool scan_recheck(PWNER::ByteMatches& writing_matches,
                      const PWNER::Edata_type& data_type,
                      const PWNER::Cuservalue *uservalue,
                      PWNER::Ematch_type match_type);

private:
    /**
     *  Usually used after scan_* methods
     */
    bool scan_fit(PWNER::ByteMatches& writing_matches);

public:
    /// Create file for storing matches
    std::shared_ptr<PROCESS::Process> handler;
    volatile bool stop_flag = false;
    volatile double scan_progress = 0;
    uintptr_t step = 1;
};


class ScannerSequential {
public:
    explicit ScannerSequential(std::shared_ptr<PROCESS::Process> handler)
    : handler(std::move(handler)) {}

    /**
     *  This is used as first scan. It scans for values and allocates memory for each match.
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_regions(std::vector<PWNER::value_t>& writing_matches,
                      const PWNER::Edata_type& data_type,
                      const PWNER::Cuservalue *uservalue,
                      const PWNER::Ematch_type& match_type);

    /**
     *  Update bytes of matches (it does not scan, it just updates value).
     *  Should be used after scan_regions().
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_update(std::vector<PWNER::value_t>& writing_matches);

    /**
     *  Remove flag for each byte if value mismatched.
     *  Should be used after scan_update().
     *
     *  Will update 'matches_t'
     */
    bool scan_recheck(std::vector<PWNER::value_t>& writing_matches,
                      const PWNER::Edata_type& data_type,
                      const PWNER::Cuservalue *uservalue,
                      PWNER::Ematch_type match_type);

private:
    /**
     *  Usually used after scan_* methods
     */
    bool scan_fit(std::vector<PWNER::value_t>& writing_matches);

public:
    /// Create file for storing matches
    std::shared_ptr<PROCESS::Process> handler;
    volatile bool stop_flag = false;
    volatile double scan_progress = 0;
    uintptr_t step = 1;
};


class ScannerAddress {
public:
    explicit ScannerAddress(std::shared_ptr<PROCESS::IOMapped> handler)
    : handler(std::move(handler)) {}

    /**
     *  This is used as first scan. It scans for values and allocates memory for each match.
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_regions(std::vector<std::pair<uintptr_t,uintptr_t>>& writing_matches,
                      const uintptr_t& from,
                      const uintptr_t& to);

    /**
     *  Update bytes of matches (it does not scan, it just updates value).
     *  Should be used after scan_regions().
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_update(std::vector<uintptr_t[2]>& writing_matches);

    /**
     *  Remove flag for each byte if value mismatched.
     *  Should be used after scan_update().
     *
     *  Will update 'matches_t'
     */
    bool scan_recheck(std::vector<uintptr_t[2]>& writing_matches,
                      const uintptr_t& uservalue);

private:
    /**
     *  Usually used after scan_* methods
     */
    bool scan_fit(std::vector<uintptr_t[2]>& writing_matches);

public:
    /// Create file for storing matches
    std::shared_ptr<PROCESS::IOMapped> handler;
    volatile bool stop_flag = false;
    volatile double scan_progress = 0;
    uintptr_t step = 1;
};

NAMESPACE_END(PWNER)
