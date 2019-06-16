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

//FIXME[critical]: move useless includes to sources
#include <pwner/external.hh>
#include <pwner/common.hh>
#include <pwner/process/IO/IO.hh>
#include <pwner/process/IO/IOProcfs.hh>
#include <pwner/process/IO/IOCRIU.hh>
#include <pwner/process/IO/IOMappedHeap.hh>
#include <pwner/process/IO/IOMappedFile.hh>
//
#include <pwner/scanner/Value.hh>
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


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class Process : virtual public IO {
public:
    uintptr_t get_call_address(uintptr_t address) const {
        uint64_t code = 0;
        if (read(address + 1, &code, sizeof(uint32_t)) == sizeof(uint32_t))
            return code + address + 5;
        return 0;
    }

    uintptr_t get_absolute_address(uintptr_t address, uintptr_t offset, uintptr_t size) const {
        uint64_t code = 0;
        if (read(address + offset, &code, sizeof(uint32_t)) == sizeof(uint32_t)) {
            return address + code + size;
        }
        return 0;
    }
};


class ProcessProcfs : virtual public Process, virtual public IOProcfs {
public:
    explicit ProcessProcfs(IOCRIU& io)
    : IOProcfs(io.restore()) {}

    explicit ProcessProcfs(pid_t pid)
    : IOProcfs(pid) {}

    explicit ProcessProcfs(const std::string& regex_pattern_cmdline)
    : IOProcfs(regex_pattern_cmdline) {}
};


class ProcessHeap : virtual public Process, virtual public IOMappedHeap {
public:
    explicit ProcessHeap(const IO &io)
    : IOMappedHeap(io) {}
};


class ProcessFile : virtual public Process, virtual public IOMappedFile {
public:
    explicit ProcessFile(const IO& io, const std::filesystem::path& path)
    : IOMappedFile(io, path) {}

    explicit ProcessFile(const std::filesystem::path& path)
    : IOMappedFile(path) {}
};


class ProcessCRIU : virtual public Process, virtual public IOCRIU {
public:
    explicit ProcessCRIU(const IOProcfs& io, const std::filesystem::path& path)
    : IOCRIU(path) {
        IOCRIU::set_pid(io.pid());
        IOCRIU::dump();
    }

    explicit ProcessCRIU(const std::filesystem::path& path)
    : IOCRIU(path) {}
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
