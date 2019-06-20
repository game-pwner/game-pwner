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
