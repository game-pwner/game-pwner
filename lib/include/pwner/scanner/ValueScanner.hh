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

#include <pwner/process/Process.hh>
#include <pwner/process/IO/IO.hh>
#include <pwner/scanner/Value.hh>
#include <pwner/scanner/PredicateScanner.hh>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(SCANNER)


using predicate_t = flag(*)(const a64 *mem,
                            size_t size,
                            const match *old_match,
                            const user_value& user_value);

predicate_t get_predicate(match_type_t mt,
                          const user_value& user_value);



class ScannerSequential {
public:
    explicit ScannerSequential(const PROCESS::Process& proc)
    : proc(proc) {}

    /**
     *  This is used as first scan. It scans for values and allocates memory for each match.
     *
     *  Will read process memory and update 'matches_t'
     */
    bool scan_regions(std::vector<match>& writing_matches,
                      const user_value& uservalue,
                      predicate_t pred,
                      size_t step = 1) {
        for (const PROCESS::Region& region : proc.regions) {
            size_t region_beg = region.address;
            size_t region_end = region.address + region.size;
            {
                // У каждого потока есть свой кеш
                ::PWNER::SCANNER::IOCached cachedReader{proc};
                for(uintptr_t reg_pos = region_beg; reg_pos < region_end; reg_pos += step) {
                    a64 mem{};
                    flag type = uservalue.vars[0].type;
                    uint64_t copied = cachedReader.read(reg_pos, &mem, sizeof(a64));
                    // uint64_t copied = proc.read(reg_pos, &mem, sizeof(a64));
                    if UNLIKELY(copied == PROCESS::IO::npos) {
                        std::cout<<"err: "<<HEX(reg_pos)<<", region: "<<region<<", errno: "<<std::strerror(errno)<<std::endl;
                        break;
                    }

                    /* check if we have a match */
                    if (type &= pred(&mem, copied, nullptr, uservalue)) {
                        writing_matches.emplace_back(reg_pos, mem, type);
                    }
                }
            }
        }
        return true;
    }

    /**
     *  Update bytes of matches (it does not scan, it just updates value).
     *  Should be used after scan_regions().
     *
     *  Will read process memory and update 'matches_t'
     */
    bool matches_update(std::vector<match>& writing_matches,
                        const user_value& uservalue,
                        predicate_t pred) {
        IOCached reader(proc);

        // Invalidate cache to get fresh values
        for (match& m : writing_matches) {
            a64 mem{};
            const size_t copied = reader.read(m.address, &m.value, sizeof(m.value));
            // const size_t copied = proc.read(m.address, &mem, sizeof(mem));
            if UNLIKELY(copied == PROCESS::IO::npos) {
                m.type = flag_t::none;
            } else if UNLIKELY(copied < sizeof(m.value)) {
                // go ahead with the partial read and stop the gathering process
                if (copied <= 7) m.type &= ~flag_t::n64;
                if (copied <= 6) m.type &= ~flag_t::n64;
                if (copied <= 5) m.type &= ~flag_t::n64;
                if (copied <= 4) m.type &= ~flag_t::n64;
                if (copied <= 3) m.type &= ~flag_t::n32;
                if (copied <= 2) m.type &= ~flag_t::n32;
                if (copied <= 1) m.type &= ~flag_t::n16;
                if (copied == 0) m.type = flag_t::none;
            }
            m.type &= pred(&mem, copied, &m, uservalue);
            m.value = mem;
        }

        writing_matches.erase(std::remove_if(writing_matches.begin(), writing_matches.end(), [&](match& m) -> bool {
            return m.type == flag_t::none;
        }), writing_matches.end());
        return true;
    }

public:
    /// Create file for storing matches
    const PROCESS::Process& proc;
};


// class ScannerAddress {
// public:
//     explicit ScannerAddress(std::shared_ptr<PROCESS::IOMapped> handler)
//     : handler(std::move(handler)) {}
//
//     /**
//      *  This is used as first scan. It scans for values and allocates memory for each match.
//      *
//      *  Will read process memory and update 'matches_t'
//      */
//     bool scan_regions(std::vector<std::pair<uintptr_t,uintptr_t>>& writing_matches,
//                       const uintptr_t& from,
//                       const uintptr_t& to);
//
//     /**
//      *  Update bytes of matches (it does not scan, it just updates value).
//      *  Should be used after scan_regions().
//      *
//      *  Will read process memory and update 'matches_t'
//      */
//     bool scan_update(std::vector<uintptr_t[2]>& writing_matches);
//
//     /**
//      *  Remove flag for each byte if value mismatched.
//      *  Should be used after scan_update().
//      *
//      *  Will update 'matches_t'
//      */
//     bool scan_recheck(std::vector<uintptr_t[2]>& writing_matches,
//                       const uintptr_t& uservalue);
//
// private:
//     /**
//      *  Usually used after scan_* methods
//      */
//     bool scan_fit(std::vector<uintptr_t[2]>& writing_matches);
//
// public:
//     /// Create file for storing matches
//     std::shared_ptr<PROCESS::IOMapped> handler;
//     volatile bool stop_flag = false;
//     volatile double scan_progress = 0;
//     uintptr_t step = 1;
// };

NAMESPACE_END(SCANNER)
NAMESPACE_END(PWNER)
