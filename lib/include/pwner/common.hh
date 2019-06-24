//
// Created by root on 31.07.18.
//

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdint.h>

#if !defined(NAMESPACE_BEGIN)
#  define NAMESPACE_BEGIN(name) namespace name {
#endif
#if !defined(NAMESPACE_END)
#  define NAMESPACE_END(name) }
#endif

#if defined(__GNUC__)
#  define LIKELY(x)     (__builtin_expect(!!(x), 1))
#  define UNLIKELY(x)   (__builtin_expect(!!(x), 0))
#else
#  define LIKELY(x)     (x)
#  define UNLIKELY(x)   (x)
#  warning "This compiler not supported yet"
#endif

#ifndef _PYBIND11

#include <type_traits>
/**
 * Description: Enum class now works like enum. Defined for: ~, &, |, ^, &=, |=, ^=.
 * Performance: performance not affected if used -O3 flag, but a bit slower if -O0 used (other flags unchecked).
 */
//https://softwareengineering.stackexchange.com/questions/194412/using-scoped-enums-for-bit-flags-in-c
//https://paste.ubuntu.com/23884820/
//see also https://softwareengineering.stackexchange.com/revisions/205567/1
/*
template <typename E, typename U = typename std::underlying_type<E>::type>
static inline E operator ~(E lhs) { return static_cast<E>(~static_cast<U>(lhs)); }
template <typename E, typename U = typename std::underlying_type<E>::type>
static inline bool operator !(E lhs) { return static_cast<U>(lhs) != 0; }
#define LIFT_ENUM_OP(op, assignop) \
    template <typename E, typename U = typename std::underlying_type<E>::type> \
    static inline E operator op(E lhs, E rhs) { return static_cast<E>(static_cast<U>(lhs) op static_cast<U>(rhs)); } \
    template <typename E, typename U = typename std::underlying_type<E>::type> \
    static inline E& operator assignop(E& lhs, E rhs) { lhs = lhs op rhs; return lhs; }
LIFT_ENUM_OP(&,&=)
LIFT_ENUM_OP(|,|=)
LIFT_ENUM_OP(^,^=)
#undef LIFT_ENUM_OP
*/

#endif

#ifndef FORCE_INLINE
# if __MSVC__ // _MSC_VER
#  define FORCE_INLINE __forceinline
# elif __INTEL_COMPILER
#  define FORCE_INLINE inline __attribute__((always_inline))
//#  define FORCE_INLINE _Pragma("forceinline")
# elif __GNUC__
#  define FORCE_INLINE inline __attribute__((always_inline))
# else
#  define FORCE_INLINE inline
#  warning "This compiler not supported yet"
# endif
#endif

#ifndef NEVER_INLINE
# if __MSVC__ // _MSC_VER
#  warning "Not implemented"
# elif __INTEL_COMPILER
#  define NEVER_INLINE __attribute__((noinline))
//#  define NEVER_INLINE _Pragma("noinline")
# elif __GNUC__
#  define NEVER_INLINE __attribute__((noinline))
# else
#  define NEVER_INLINE
#  warning "This compiler not supported yet"
# endif
#endif


static inline void oom_score_adj(ssize_t x) noexcept {
    if (std::fstream f{"/proc/self/oom_score_adj", std::ios::out}; f.is_open())
        f << std::to_string(x);
}

using u8  = uint8_t;
using s8  =  int8_t;
using u16 = uint16_t;
using s16 =  int16_t;
using u32 = uint32_t;
using s32 =  int32_t;
using u64 = uint64_t;
using s64 =  int64_t;
using f32 = float;
using f64 = double;

union a64 {
    ::u8  u8;
    ::s8  s8;
    ::u16 u16;
    ::s16 s16;
    ::u32 u32;
    ::s32 s32;
    ::u64 u64;
    ::s64 s64;
    ::f32 f32;
    ::f64 f64;
    ::u8 data[sizeof(::u64)];
};

/** @arg what: any number
 * @return: string number represented as hex */
template <typename T, size_t value_size = sizeof(T), std::endian endianess = std::endian::native>
std::string HEX(const T& value) {
    using namespace std;
    auto *buffer = (uint8_t *)(&value);
    char converted[value_size * 2 + 1];
    if (endianess == std::endian::big)
        for(size_t i = 0; i < value_size; ++i) {
            sprintf(&converted[i*2], "%02X", buffer[i]);
        }
    else
        for(size_t i = 0; i < value_size; ++i) {
            sprintf(&converted[i*2], "%02X", buffer[value_size-1-i]);
        }
    return converted;
}

class Timer {
public:
    explicit Timer(std::string what)
            : m_what(std::move(what)), m_tp(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        std::clog << m_what << ": done in " << std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::high_resolution_clock::now() - m_tp).count() << " seconds" << std::endl;
    }

private:
    std::string m_what;
    std::chrono::high_resolution_clock::time_point m_tp;
};
