/*
    This file is part of Reverse Engine.

    Find process by PID or title, access it's address space, change any
    value you need.

    Copyright (C) 2017-2018 Ivan Stepanov <ivanstepanovftw@gmail.com>

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

#include <sys/uio.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <pwner/scanner/Value.hh>
#include <pwner/external.hh>
#include <pwner/common.hh>
#include <pwner/process/IO.hh>
#include <pwner/process/handlers/Procfs.hh>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class Process : virtual public IO {
public:
    uintptr_t get_call_address(uintptr_t address) {
        uint64_t code = 0;
        if (read(address + 1, &code, sizeof(uint32_t)) == sizeof(uint32_t))
            return code + address + 5;
        return 0;
    }

    uintptr_t get_absolute_address(uintptr_t address, uintptr_t offset, uintptr_t size) {
        uint64_t code = 0;
        if (read(address + offset, &code, sizeof(uint32_t)) == sizeof(uint32_t)) {
            return address + code + size;
        }
        return 0;
    }
};

class ProcessProcfs : public Process, public IOProcfs {
public:
    explicit ProcessProcfs(pid_t pid)
    : IOProcfs(pid) { }

    explicit ProcessProcfs(const std::regex& regex_pattern_cmdline)
    : IOProcfs(regex_pattern_cmdline) { }
};


class ProcessHeap : public Process, public IOMappedHeap {
public:
    explicit ProcessHeap(IO &io)
    : IOMappedHeap(io) { }
};

// class ProcessFile : public Process, public IOMappedFile {
// public:
//     explicit ProcessFile(const IO &io)
//     : IOMappedFile(io) { }
//
//     explicit ProcessFile(const std::filesystem::path &file)
//     : IOMappedFile(file) { }
// };

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
