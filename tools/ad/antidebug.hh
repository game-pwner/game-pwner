/*
    This file is part of PWNER.

    Copyright (C) 2017-2019 Ivan Stepanov <ivanstepanovftw@gmail.com>

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

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include <sys/ptrace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <sys/auxv.h> // auxval
#include <sys/utsname.h>
#include <dlfcn.h>
#include <limits>

#if !defined __amd64__ && \
    !defined __arm__ && \
    !defined __aarch64__ && \
    !defined __i386__
#error "Compiling for unknown architecture. Only x86(-64) and ARMV[78] are supported."
#endif

namespace antidebug {

enum result {
    none,
    yes,
    no,
};

extern unsigned char *_start;
extern unsigned char *__etext;

extern long _r_debug; // /usr/lib/ld-2.29.so

class arch {
public:
    enum type {
        none = 0u,
        AMD64,
        I386,
        ARM64,
        ARMV7,
    } value;

    /* current machine */
    explicit arch() noexcept {
        value = type::none;
        std::string auxval = (char *) getauxval(AT_PLATFORM);
        for (auto e : {type::AMD64, type::I386, type::ARM64, type::ARMV7}) {
            if (to_string(e) == auxval)
                value = e;
        }
    }

    static constexpr const char *to_string(arch::type v) {
        switch (v) {
            case type::AMD64: return "x86_64";
            case type::I386:  return "i686";
            case type::ARM64: return "aarch64";
            case type::ARMV7: return "v7l";
            default: return nullptr;
        }
    }

    std::string str() {
        switch (value) {
            case type::AMD64: return "x86_64";
            case type::I386:  return "i686";
            case type::ARM64: return "aarch64";
            case type::ARMV7: return "v7l";
            default: return "";
        }
    }

    std::string ret_ldhook() {
        switch (value) {
            case type::AMD64: return "\xf3\xc3";           /* rep ret */
            case type::I386:  return "\xf3\xc3";           /* rep ret */
            case type::ARM64: return "\xc0\x03\x5f\xd6";   /* ret     */
            case type::ARMV7: return "\x1e\xff\x2f\xe1";   /* bx lr   */
            default: return "";
        }
    }

    /* 64 or 32 bit ELF running on a 64 bit kernel */
    constexpr uint64_t elf_bits() const {
        switch (value) {
            case type::AMD64: return 0x0000555555000000ULL;
            case type::I386:  return 0x0000000056550000ULL;
            case type::ARM64: return 0x0000aaaaaa000000ULL;
            case type::ARMV7: return 0x00000000aaaa0000ULL;
            default: return 0;
        }
    }

    /* 32 bit ELF running on a 32 bit kernel. */
    constexpr uint64_t elf_bits_native() const {
        switch (value) {
            case type::I386:  return 0x0000000000400000ULL;
            case type::ARMV7: return 0x0000000000400000ULL;
            default: return result::none;
        }
    }

    constexpr uint64_t elf_mask() const {
        switch (value) {
            case type::AMD64: return 0x0000ffffff000000ULL;
            case type::I386:  return 0x00000000ffff0000ULL;
            case type::ARM64: return 0x0000ffffff000000ULL;
            case type::ARMV7: return 0x00000000ffff0000ULL;
            default: return result::none;
        }
    }

    /* 64 or 32 bit shared library running on a 64 bit kernel */
    constexpr uint64_t lib_bits() const {
        switch (value) {
            case type::AMD64: return 0x00007ffff7000000ULL;
            case type::I386:  return 0x00000000f7f00000ULL;
            case type::ARM64: return 0x0000ffffb7000000ULL;
            case type::ARMV7: return 0x00000000f7700000ULL;
            default: return result::none;
        }
    }

    /* 32 bit shared library running on a 32 bit kernel */
    constexpr uint64_t lib_bits_native() const {
        switch (value) {
            case type::I386:  return 0x00000000b7f00000ULL;
            case type::ARMV7: return 0x0000000076f00000ULL;
            default: return result::none;
        }
    }

    constexpr uint64_t lib_mask() const {
        switch (value) {
            case type::AMD64: return 0x0000ffffff000000ULL;
            case type::I386:  return 0x00000000fff00000ULL;
            case type::ARM64: return 0x0000ffffff000000ULL;
            case type::ARMV7: return 0x00000000fff00000ULL;
            default: return result::none;
        }
    }
};

/* Current arch */
arch this_arch;

/* Is Address space layout randomization active */
ssize_t aslr_active() {
    std::fstream fp("/proc/sys/kernel/randomize_va_space", std::ios::in);
    if (!fp.good())
        return result::no;
    uint64_t aslr_state;
    fp >> aslr_state;
    if (aslr_state == 0)
        return result::no;
    return result::yes;
}


/* try ptrace self */
ssize_t is_ptrace() {
    if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) {
        return result::yes;
    }
    ptrace(PTRACE_DETACH, 0, nullptr, nullptr);
    return result::no;
}


/* measure distance of vdso and stack */ // TODO not tested
ssize_t is_vdso() {
    unsigned long tos;
    unsigned long vdso = getauxval(AT_SYSINFO_EHDR);

    if (this_arch.value == arch::type::ARM64 || this_arch.value == arch::type::I386) {
        /* This trick unfortunately does not work on 32bit x86 and ARM64 */
        return result::none;
    }

    if (!vdso) {
        /* Auxiliary vector does not contain vdso entry. Unknown result. */
        return result::none;
    }

    if (aslr_active() != 1) {
        /* No ASLR on this machine. Unknown result */
        return result::none;
    }

    if ((unsigned long) &tos > vdso)
        return result::yes;
    else
        return result::no;
}


/* Program headers are close enough to the beginning of the ELF to be
 * representative for the ELF's base address. We use ld_base because it's
 * the simplest way to obtain an address of some shared library. */ // TODO not tested
ssize_t is_noaslr() {
    unsigned long elf = getauxval(AT_PHDR) & ~((unsigned long) getpagesize() - 1);
    unsigned long ld = getauxval(AT_BASE) & ~((unsigned long) getpagesize() - 1);
    struct utsname utsname;

    if (!elf || !ld) {
        return result::none;
    }

    if (aslr_active() != 1) {
        /* No ASLR on this machine. Unknown result */
        return result::none;
    }

    if (uname(&utsname) == -1) {
        return result::none;
    }

    switch (this_arch.value) {
        case arch::type::AMD64:
        case arch::type::ARM64:
            if (((elf & this_arch.elf_mask()) == this_arch.elf_bits())
            && ((ld & this_arch.lib_mask()) == this_arch.lib_bits()))
                return result::yes;
            return result::no;
        case arch::type::I386:
            if (!strcmp(utsname.machine, arch::to_string(arch::type::AMD64))) {
                /* 32 bit binary running on 64 bit kernel */
                if (((elf & this_arch.elf_mask()) == this_arch.elf_bits())
                && ((ld & this_arch.lib_mask()) == this_arch.lib_bits()))
                    return result::yes;
                return result::no;
            } else {
                /* 32 bit binary running on 32 bit kernel */
                if (((elf & this_arch.elf_mask()) == this_arch.elf_bits_native())
                    && ((ld & this_arch.lib_mask()) == this_arch.lib_bits_native()))
                    return result::yes;
                return result::no;
            }
        case arch::type::ARMV7:
            if (!strcmp(utsname.machine, arch::to_string(arch::type::ARM64))) {
                /* 32 bit binary running on 64 bit kernel */
                if (((elf & this_arch.elf_mask()) == this_arch.elf_bits())
                && ((ld & this_arch.lib_mask()) == this_arch.lib_bits()))
                    return result::yes;
                return result::no;
            } else {
                /* 32 bit binary running on 32 bit kernel */
                if (((elf & this_arch.elf_mask()) == this_arch.elf_bits_native())
                && ((ld & this_arch.lib_mask()) == this_arch.lib_bits_native()))
                    return result::yes;
                return result::no;
            }
        default:
            return result::none;
    }
}


/* gdb does not provide env variables LINES and COLUMNS */
ssize_t is_env() {
    if (getenv("LINES") || getenv("COLUMNS"))
        return result::yes; /* Debatable */
    return result::no;
}


/* check if something preloaded */
ssize_t is_ld_preload() {
    if (getenv("LD_PRELOAD"))
        return result::yes;
    return result::no;
}

ssize_t is_parent_debugger() {
    //todo[low]: recursive
    pid_t ppid = getppid();
    std::error_code ec;
    auto parent_path = std::filesystem::read_symlink("/proc/"+std::to_string(ppid)+"/exe", ec);
    if (ec)
        return result::none;
    if (parent_path.filename() == "gdb")
        return result::yes;
    if (parent_path.filename() == "lldb")
        return result::yes;
    if (parent_path.filename() == "strace")
        return result::yes;
    if (parent_path.filename() == "ltrace")
        return result::yes;
    return result::no;
}

ssize_t is_ldhook() {
    // https://github.com/lattera/glibc/blob/master/elf/dl-debug.c
    int ret = memcmp((void *) *(&_r_debug + 2),
                     &this_arch.ret_ldhook()[0],
                     this_arch.ret_ldhook().size());
    if (ret)
        return result::yes;
    return result::no;
} // FIXME always returns true


/* GDB relocates the heap to the end of the bss section */
ssize_t is_nearheap() {
    static unsigned char bss;
    std::vector<uint8_t> probe;
    probe.resize(0x10);

    if (&probe[0] - &bss <= 0x20000) {
        return result::yes;
    }

    return result::no;
}

ssize_t is_breakpoint() {
    char *start = reinterpret_cast<char *>(&_start);
    char *end =  reinterpret_cast<char *>(&__etext);
    while (start != end) {
        if (((*(volatile unsigned *) start) & 0xFFu) == 0xCCu) {
            return result::yes;
        }
        start++;
    }
    return result::no;
}

std::vector<uintptr_t> get_breakpoints() {
    std::vector<uintptr_t> result;
    char *start = reinterpret_cast<char *>(&_start);
    char *end =  reinterpret_cast<char *>(&__etext);
    while (start != end) {
        if (((*(volatile unsigned *) start) & 0xFFu) == 0xCCu) {
            result.emplace_back(*reinterpret_cast<uintptr_t*>(&start));
        }
        start++;
    }
    return result;
}

ssize_t is_valgrind() {
    /// malloc по-дефолту в [stack], под валгриндом - кое-где
    // TODO[windows]: Virtualalloc под виндярой
    using namespace std;
    std::string symbol_to_load = "malloc";
    typedef void (*this_malloc_t)();
    this_malloc_t this_malloc = (this_malloc_t) dlsym(RTLD_NEXT, symbol_to_load.c_str());
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        return result::none;
    }
    uintptr_t address_malloc = reinterpret_cast<uintptr_t>(reinterpret_cast<uintptr_t *>(&this_malloc));

    std::ifstream maps("/proc/self/maps", std::ios::in);
    std::string line;
    while (getline(maps, line)) {
        std::istringstream ss(line);
        std::uintptr_t address_begin;
        std::uintptr_t address_end;
        std::string skip_string;
        char skip_c;
        std::string path;
        if (ss >> hex >> address_begin >> skip_c >> address_end
        >> skip_string >> skip_string >> skip_string >> skip_string >> ws) {
            getline(ss, path);
            if (address_begin <= address_malloc && address_malloc < address_end)
                if (path == "[stack]")
                    return result::no;
        }
    }

    return result::yes;
}

ssize_t is_perf() {
    return result::none;
} // TODO: [1] perf detection (КАК?), [2] vtune detection

} // namespace antidebug
