/*
    This file is part of Reverse Engine.



    Copyright (C) 2017-2018 Ivan Stepanov <ivanstepanovftw@gmail.com>
    Copyright (C) 2015,2017 Sebastian Parschauer <s.parschauer@gmx.de>
    Copyright (C) 2010      WANG Lu  <coolwanglu(a)gmail.com>
    Copyright (C) 2009      Eli Dupree  <elidupree(a)charter.net>

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

#include <pwner/scanner/Value.hh>
#include <pwner/scanner/ValueScanner.hh>
#include <pwner/common.hh>

using namespace PWNER;

bool
ScannerSequential::scan_regions(std::vector<value_t>& writing_matches,
                                       const Edata_type& data_type,
                                       const Cuservalue *uservalue,
                                       const Ematch_type& match_type) {
    using namespace std;
    using namespace std::chrono;

    if (!sm_choose_scanroutine(data_type, match_type, uservalue, false)) {
        printf("unsupported scan for current data type.\n");
        return false;
    }

    scan_progress = 0.0;
    stop_flag = false;


    for(const PROCESS::Region& region : handler->regions) {
        size_t region_beg = region.address;
        size_t region_end = region.address + region.size;

        /* For every offset, check if we have a match. */
        // #pragma omp parallel firstprivate(data_type, uservalue, match_type, region_beg, region_end), shared(writing_matches)
        {
            // У каждого потока есть свой кеш
            PROCESS::IOCached cachedReader(*handler);
            std::vector<value_t> res; //todo[high] или лучше сразу в writing_matches писать? по идее - да

            // #pragma omp for
            for(uintptr_t reg_pos = region_beg; reg_pos < region_end; reg_pos += step) {
                mem64_t memory_ptr{};
                uint64_t copied = cachedReader.read(reg_pos, &memory_ptr, sizeof(mem64_t));
                if UNLIKELY(copied == PROCESS::IO::npos) {
                    // cout<<"err: "<<HEX(reg_pos)<<", region: "<<region<<", errno: "<<std::strerror(errno)<<endl;
                    // errno=0;
                    // break; // В однопотоке тут будет break;
                    continue;
                }
                flag checkflags;

                /* check if we have a match */
                size_t match_length = (*sm_scan_routine)(&memory_ptr, copied, nullptr, uservalue, checkflags);

                if UNLIKELY(match_length > 0) {
                    // #pragma omp critical
                    res.emplace_back(reg_pos, memory_ptr, checkflags);
                }
            }
            #pragma omp critical
            writing_matches.insert(writing_matches.end(), res.begin(), res.end());
        }
    }

    scan_fit(writing_matches);
    scan_progress = 1.0;
    return true;
}


bool ScannerSequential::scan_update(std::vector<value_t>& writing_matches) {
    PROCESS::IOCached reader(*handler);

    // Invalidate cache to get fresh values
    for (value_t& s : writing_matches) {
        //const size_t copied = (*handler).read(s.address, &s.mem, sizeof(s.mem));
        const size_t copied = reader.read(s.address, &s.mem, sizeof(s.mem));
        /* check if the read succeeded */
        if UNLIKELY(copied == PROCESS::IO::npos) {
            s.flags = flag_t::flags_empty;
        } else if UNLIKELY(copied < sizeof(s.mem)) {
            /* go ahead with the partial read and stop the gathering process */
            if (copied <= 7) s.flags &= ~flag_t::flags_64b;
            if (copied <= 6) s.flags &= ~flag_t::flags_64b;
            if (copied <= 5) s.flags &= ~flag_t::flags_64b;
            if (copied <= 4) s.flags &= ~flag_t::flags_64b;
            if (copied <= 3) s.flags &= ~flag_t::flags_32b;
            if (copied <= 2) s.flags &= ~flag_t::flags_32b;
            if (copied <= 1) s.flags &= ~flag_t::flags_16b;
            if (copied == 0) s.flags = flag_t::flags_empty;
        }
    }
    scan_fit(writing_matches);
    return true;
}


bool
ScannerSequential::scan_recheck(std::vector<value_t>& writing_matches,
                                const Edata_type& data_type,
                                const Cuservalue *uservalue,
                                Ematch_type match_type) {
    using namespace std;

    if (!sm_choose_scanroutine(data_type, match_type, uservalue, false)) {
        printf("unsupported scan for current data type.\n");
        return false;
    }

    scan_progress = 0.0;
    stop_flag = false;

    for (value_t& val : writing_matches) {
        size_t mem_size = val.flags.memlength(data_type);
        flag checkflags;
        unsigned int match_length = (*sm_scan_routine)(&val.mem, mem_size, &val, uservalue, checkflags);
        val.flags = checkflags;
    }
    scan_fit(writing_matches);

    return true;
}


bool ScannerSequential::scan_fit(std::vector<value_t>& writing_matches) {
    // Invalidate cache to get fresh values
    std::vector<value_t> result;
    result.reserve(writing_matches.size());
    for (const value_t& v : writing_matches) {
        if (v.flags == flag_t::flags_empty)
            continue;
        result.push_back(v);
    }
    result.shrink_to_fit();

    writing_matches = std::move(result);
    return true;
}
