//
// Created by root on 30.04.19.
//

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
#include <sys/auxv.h>
#include <sys/utsname.h>
#include <dlfcn.h>
#include <limits>

#if !defined __amd64__ && \
    !defined __arm__ && \
    !defined __aarch64__ && \
    !defined __i386__
#error "Compiling for unknown architecture. Only x86(-64) and ARMV[78] are supported."
#endif

extern unsigned char *_start;
extern unsigned char *__etext;

extern long _r_debug; // /usr/lib/ld-2.29.so


class Arch {
public:
    enum class Value : uint8_t {
        none = 0u,
        AMD64,
        I386,
        ARM64,
        ARMV7,
    };

    explicit Arch() {
        std::string auxval = (char *) getauxval(AT_PLATFORM);
        for (auto e : {Value::AMD64, Value::I386, Value::ARM64, Value::ARMV7}) {
            if (to_string(e) == auxval)
                value = e;
        }
    }

    static constexpr const char *to_string(Arch::Value v) {
        switch (v) {
            case Value::AMD64: return "x86_64";
            case Value::I386:  return "i686";
            case Value::ARM64: return "aarch64";
            case Value::ARMV7: return "v7l";
            default: return nullptr;
        }
    }

    std::string str() {
        switch (value) {
            case Value::AMD64: return "x86_64";
            case Value::I386:  return "i686";
            case Value::ARM64: return "aarch64";
            case Value::ARMV7: return "v7l";
            default: return "";
        }
    }

    std::string ret_ldhook() {
        switch (value) {
            case Value::AMD64: return "\xf3\xc3";           /* rep ret */
            case Value::I386:  return "\xf3\xc3";           /* rep ret */
            case Value::ARM64: return "\xc0\x03\x5f\xd6";   /* ret     */
            case Value::ARMV7: return "\x1e\xff\x2f\xe1";   /* bx lr   */
            default: return "";
        }
    }

    /* 64 or 32 bit ELF running on a 64 bit kernel */
    constexpr uint64_t elf_bits() const {
        switch (value) {
            case Value::AMD64: return 0x0000555555000000ULL;
            case Value::I386:  return 0x0000000056550000ULL;
            case Value::ARM64: return 0x0000aaaaaa000000ULL;
            case Value::ARMV7: return 0x00000000aaaa0000ULL;
            default: return 0;
        }
    }

    /* 32 bit ELF running on a 32 bit kernel. */
    constexpr uint64_t elf_bits_native() const {
        switch (value) {
            case Value::I386:  return 0x0000000000400000ULL;
            case Value::ARMV7: return 0x0000000000400000ULL;
            default: return 0;
        }
    }

    constexpr uint64_t elf_mask() const {
        switch (value) {
            case Value::AMD64: return 0x0000ffffff000000ULL;
            case Value::I386:  return 0x00000000ffff0000ULL;
            case Value::ARM64: return 0x0000ffffff000000ULL;
            case Value::ARMV7: return 0x00000000ffff0000ULL;
            default: return 0;
        }
    }

    /* 64 or 32 bit shared library running on a 64 bit kernel */
    constexpr uint64_t lib_bits() const {
        switch (value) {
            case Value::AMD64: return 0x00007ffff7000000ULL;
            case Value::I386:  return 0x00000000f7f00000ULL;
            case Value::ARM64: return 0x0000ffffb7000000ULL;
            case Value::ARMV7: return 0x00000000f7700000ULL;
            default: return 0;
        }
    }

    /* 32 bit shared library running on a 32 bit kernel */
    constexpr uint64_t lib_bits_native() const {
        switch (value) {
            case Value::I386:  return 0x00000000b7f00000ULL;
            case Value::ARMV7: return 0x0000000076f00000ULL;
            default: return 0;
        }
    }

    constexpr uint64_t lib_mask() const {
        switch (value) {
            case Value::AMD64: return 0x0000ffffff000000ULL;
            case Value::I386:  return 0x00000000fff00000ULL;
            case Value::ARM64: return 0x0000ffffff000000ULL;
            case Value::ARMV7: return 0x00000000fff00000ULL;
            default: return 0;
        }
    }

public:
    Value value;
};

class AC {
public:
    ssize_t aslr_active() {
        std::fstream fp("/proc/sys/kernel/randomize_va_space", std::ios::in);
        if (!fp.good())
            return -1;
        uint64_t aslr_state;
        fp >> aslr_state;
        if (aslr_state == 0)
            return -1;
        return 1;
    }

    ssize_t is_ptrace() {
        if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) {
            return 1;
        }
        ptrace(PTRACE_DETACH, 0, nullptr, nullptr);
        return -1;
    }

    ssize_t is_vdso() {
        /* measure distance of vdso and stack */
        unsigned long tos;
        unsigned long vdso = getauxval(AT_SYSINFO_EHDR);

        if (this_arch.value == Arch::Value::ARM64 || this_arch.value == Arch::Value::I386) {
            /* This trick unfortunately does not work on 32bit x86 and ARM64 */
            return 0;
        }

        if (!vdso) {
            /* Auxiliary vector does not contain vdso entry. Unknown result. */
            return 0;
        }

        if (aslr_active() != 1) {
            /* No ASLR on this machine. Unknown result */
            return 0;
        }

        if ((unsigned long) &tos > vdso)
            return 1;
        else
            return -1;
    } // TODO not tested

    ssize_t is_noaslr() {
        /* Program headers are close enough to the beginning of the ELF to be
         * representative for the ELF's base address. We use ld_base because it's
         * the simplest way to obtain an address of some shared library. */
        unsigned long elf = getauxval(AT_PHDR) & ~((unsigned long) getpagesize() - 1);
        unsigned long ld = getauxval(AT_BASE) & ~((unsigned long) getpagesize() - 1);
        struct utsname utsname;

        if (!elf || !ld) {
            return 0;
        }

        if (aslr_active() != 1) {
            /* No ASLR on this machine. Unknown result */
            return 0;
        }

        if (uname(&utsname) == -1) {
            return 0;
        }

        switch (this_arch.value) {
            case Arch::Value::AMD64:
            case Arch::Value::ARM64:
                if (((elf & this_arch.elf_mask()) == this_arch.elf_bits())
                && ((ld & this_arch.lib_mask()) == this_arch.lib_bits()))
                    return 1;
                return -1;
            case Arch::Value::I386:
                if (!strcmp(utsname.machine, Arch::to_string(Arch::Value::AMD64))) {
                    /* 32 bit binary running on 64 bit kernel */
                    if (((elf & this_arch.elf_mask()) == this_arch.elf_bits())
                    && ((ld & this_arch.lib_mask()) == this_arch.lib_bits()))
                        return 1;
                    return -1;
                } else {
                    /* 32 bit binary running on 32 bit kernel */
                    if (((elf & this_arch.elf_mask()) == this_arch.elf_bits_native())
                        && ((ld & this_arch.lib_mask()) == this_arch.lib_bits_native()))
                        return 1;
                    return -1;
                }
            case Arch::Value::ARMV7:
                if (!strcmp(utsname.machine, Arch::to_string(Arch::Value::ARM64))) {
                    /* 32 bit binary running on 64 bit kernel */
                    if (((elf & this_arch.elf_mask()) == this_arch.elf_bits())
                    && ((ld & this_arch.lib_mask()) == this_arch.lib_bits()))
                        return 1;
                    return -1;
                } else {
                    /* 32 bit binary running on 32 bit kernel */
                    if (((elf & this_arch.elf_mask()) == this_arch.elf_bits_native())
                    && ((ld & this_arch.lib_mask()) == this_arch.lib_bits_native()))
                        return 1;
                    return -1;
                }
            default:
                return 0;
        }
    } // TODO not tested

    ssize_t is_env() {
        if (getenv("LINES") || getenv("COLUMNS"))
            return 1; /* Debatable */
        return -1;
    }

    ssize_t is_ld_preload() {
        if (getenv("LD_PRELOAD"))
            return 1;
        return -1;
    }

    ssize_t is_parent_debugger() {
        //todo use sfs
        //todo is it recursive?
        //todo is it safe to use?
        char link_name[0x100] = {0};
        size_t target_max_len = 0x100;
        ssize_t target_real_len = 0;
        int res = -1;
        char *link_target = static_cast<char *>(calloc(target_max_len, sizeof(char)));
        if (!link_target) {
            fprintf(stderr, "Out of memory in file __FILE__!");
            return 0;
        }
        pid_t parent = getppid();

        snprintf(link_name, sizeof(link_name) - 1, "/proc/%u/exe", parent);
        while ((target_real_len = readlink(link_name, link_target, target_max_len))
               == target_max_len) {
            target_max_len *= 2;
            /* BUG: This leaks a chunk of memory of size target_max_len if realloc
             * fails, but I'm willing to take this risk ... */
            link_target = static_cast<char *>(realloc(link_target, target_max_len));
            if (!link_target) {
                fprintf(stderr, "Out of memory in file __FILE__ while growing buf!");
                return 0;
            }
            memset(link_target, '\x00', target_max_len);
        }

        if (!strcmp(basename(link_target), "gdb"))
            res = 1;
        if (strstr(link_target, "lldb"))
            res = 1;
        if (!strcmp(basename(link_target), "strace"))
            res = 1;
        if (!strcmp(basename(link_target), "ltrace"))
            res = 1;

        free(link_target);
        return res;
    }

    ssize_t is_ldhook() {
        // https://github.com/lattera/glibc/blob/master/elf/dl-debug.c
        int ret = memcmp((void *) *(&_r_debug + 2),
                         &this_arch.ret_ldhook()[0],
                         this_arch.ret_ldhook().size());
        if (ret)
            return 1;
        return -1;
    } // FIXME always returns true

    ssize_t is_nearheap() {
        /* GDB relocates the heap to the end of the bss section */
        static unsigned char bss;
        std::vector<uint8_t> probe;
        probe.resize(0x10);

        if (&probe[0] - &bss <= 0x20000) {
            return 1;
        }

        return -1;
    }

    ssize_t is_breakpoint() {
        char *start = reinterpret_cast<char *>(&_start);
        char *end =  reinterpret_cast<char *>(&__etext);
        while (start != end) {
            if (((*(volatile unsigned *) start) & 0xFF) == 0xCC) { //todo warning
                return 1;
            }
            ++start;
        }
        return -1;
    }

    std::vector<std::string> get_breakpoints() { //todo not strings
        std::vector<std::string> bps;
        char *start = (char *) &_start;
        char *end = (char *) &__etext;
        while (start != end) {
            if (((*(volatile unsigned *) start) & 0xFF) == 0xCC) { //todo warning
                std::ostringstream ss;
                ss << "Breakpoint at "<<std::hex<<(void *)(start)<<": ("<<*(int *)(*start)<<")"<<std::dec; //todo
                bps.emplace_back(ss.str());
                printf("Breakpoint at %p: (%x)\n", start, *start);
            }
            ++start;
        }
        return bps;
    }

    ssize_t is_valgrind() {
        /// malloc по-дефолту в [stack], под валгриндом - в пизде
        // TODO[windows]: Virtualalloc под виндярой
        using namespace std;
        std::string symbol_to_load = "malloc";
        typedef void (*this_malloc_t)();
        this_malloc_t this_malloc = (this_malloc_t) dlsym(RTLD_NEXT, symbol_to_load.c_str());
        const char *dlsym_error = dlerror();
        if (dlsym_error) {
            return 0;
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
                        return -1;
            }
        }

        return 1;
    }

    ssize_t is_more_streams() {
        // system("ls -la /proc/self/fd");
        // system("echo 123 && readlink -f /proc/self/fd/3 && echo 1");
        return 0;
    } // TODO: https://stackoverflow.com/a/10973747/3184775

    ssize_t is_perf() {
        return 0;
    } // TODO: [1] perf detection (КАК БЛЯТЬ? ТЫ ДУРАК?), [2] vtune detection

public:
    Arch this_arch;
};
