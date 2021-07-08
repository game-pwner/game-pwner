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
#include <pwner/process/Process.hh>
#include <pwner/process/IO/IOCached.hh>
#include <pwner/scanner/Value.hh>
#include <pwner/scanner/value/match.hh>
#include <pwner/scanner/value/user_value.hh>


NAMESPACE_BEGIN(pwner)

NAMESPACE_BEGIN(predicate)
    enum class type {
        VARIABLE,
        VLA,      // variable-length array
        STRING,   // regex matcher
    };

    enum class variable_predicate {
        ANY,
        EQUAL_TO,
        NOT_EQUAL_TO,
        GRATER_THAN,
        LESS_THAN,
        GREATER_EQUAL_THAN,
        LESS_EQUAL_THAN,
        IN_RANGE,
        NOT_IN_RANGE,
        CHANGED,
        NOT_CHANGED,
        INCREASED,
        DECREASED,
        INCREASED_BY,
        DECREASED_BY,
    };

    // enum class string_predicate {
    //     EQUAL_TO_ASCII,
    //     EQUAL_TO_UTF8,
    //     EQUAL_TO_UTF16,
    //     EQUAL_TO_UTF32,
    //     MAX,
    // };

    using predicate_t = uint16_t(*)(const a64 *mem,
                                    size_t size,
                                    const match *old_match,
                                    const user_value& user_value);

    predicate_t get_variable_predicate(const user_value& user_value, variable_predicate e);
    predicate_t get_vla_predicate(const user_value& user_value);
    predicate_t get_string_predicate(const user_value& user_value);
NAMESPACE_END(predicate)


class scanner_value {
public:
    explicit scanner_value(const Process& proc)
    : proc(proc) {}

    bool scan(std::vector<match>& writing_matches,
              const user_value& uservalue,
              predicate::predicate_t pred,
              size_t step = 1) {
        IOCached reader{proc};
        for (const PROCESS::Region& region : proc.regions) {
            size_t region_beg = region.address;
            size_t region_end = region.address + region.size;
            for (uintptr_t addr = region_beg; addr < region_end;) {
                char *buf = nullptr;
                size_t size = reader.read(addr, reinterpret_cast<void**>(&buf));
                if UNLIKELY(size == PROCESS::IO::npos) {
                    std::cout<<"err: read: "<<HEX(addr)<<", region: "<<region<<", errno: "<<std::strerror(errno)<<std::endl;
                    break;
                }
                size = std::min(size, region_end - addr);
                if (size < uservalue.least) {
                    addr += step;
                } else {
                    uintptr_t addr_end_inclusive = addr + size - uservalue.least;
                    for (; addr <= addr_end_inclusive; addr += step, buf += step, size -= step) {
                        a64 *mem = reinterpret_cast<a64 *>(buf);
                        if (u16 type = pred(mem, size, nullptr, uservalue)) {
                            writing_matches.emplace_back(addr, *mem, type);
                        }
                    }
                }
            }
        }
        return true;
    }

    bool rescan(std::vector<match>& writing_matches,
                const user_value& uservalue,
                predicate::predicate_t pred) {
        IOCached reader{proc};

        for (match& m : writing_matches) {
            a64 mem{};
            size_t size = reader.read(m.address, &mem, sizeof(mem));
            // const size_t size = proc.read(m.address, &mem, sizeof(mem));
            if UNLIKELY(size == PROCESS::IO::npos) {
                m.type = 0;
            }
            if (size < uservalue.least) {
                m.type = 0;
            } else {
                m.type &= pred(&mem, size, &m, uservalue);
                m.value = mem;
            }
        }

        writing_matches.erase(std::remove_if(writing_matches.begin(), writing_matches.end(), [&](match& m) -> bool {
            return m.type == 0;
        }), writing_matches.end());
        return true;
    }

public:
    const Process& proc;
};

//todo предикатом обойдёшься
class ScannerAddress {
public:
    explicit ScannerAddress(PROCESS::IOMapped& proc)
    : proc(proc) {}

    /**
     *  This is used as first scan. It scans for values and allocates memory for each match.
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_regions(std::vector<std::pair<uintptr_t,uintptr_t>>& writing_matches,
                      const uintptr_t& from,
                      const uintptr_t& to);


public:
    /// Create file for storing matches
    PROCESS::IOMapped& proc;
    volatile bool stop_flag = false;
    volatile double scan_progress = 0;
    uintptr_t step = 1;
};

bool
ScannerAddress::scan_regions(std::vector<std::pair<uintptr_t,uintptr_t>>& writing_matches,
                             const uintptr_t& from,
                             const uintptr_t& to) {
    // todo[med]: uservalue hardcoded to 8 byte
    step = 4;
    using namespace std;
    using namespace std::chrono;

    for(size_t i = 0; i < proc.regions.size(); i++) {
        char *start = proc.regions_mapped[i].get();
        char *finish = start + proc.regions[i].size - 7;

        for (char *cursor = start; cursor < finish; cursor += step) {
            a64 *mem = (a64 *) cursor;
            if UNLIKELY(mem->u64 >= from && mem->u64 <= to) {
                uintptr_t address = proc.regions[i].address + (cursor - start);
                writing_matches.emplace_back(address, mem->u64);
            }
        }
    }

    return true;
}


NAMESPACE_END(pwner)
