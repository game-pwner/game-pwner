/*
    This file is part of Reverse Engine.

    Structs for target process.

    Copyright (C) 2017-2018 Ivan Stepanov <ivanstepanovftw@gmail.com>
    Copyright (C) 2017      Andrea Stacchiotti <andreastacchiotti@gmail.com>

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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/param.h>
#include <bitset>
#include <cstddef>
#include <bitmask/bitmask.hpp>
#include <pwner/common.hh>
#include <boost/lexical_cast.hpp>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(SCANNER)

// enum class data_type_t : uint16_t {
//     ANYNUMBER,              /* ANYINTEGER or ANYFLOAT */
//     ANYINTEGER,             /* INTEGER of whatever width */
//     ANYFLOAT,               /* FLOAT of whatever width */
//     INTEGER8,
//     INTEGER16,
//     INTEGER32,
//     INTEGER64,
//     FLOAT32,
//     FLOAT64,
//     BYTEARRAY,
//     STRING
// };
// BITMASK_DEFINE_VALUE_MASK(data_type_t, 0xffff)
//
enum class match_type_t
{
    MATCHANY,                /* for snapshot */
    /* following: compare with a given value */
    MATCHEQUALTO,
    MATCHNOTEQUALTO,
    MATCHGREATERTHAN,
    MATCHLESSTHAN,
    MATCHRANGE,
    MATCHEXCLUDE,
    /* following: compare with the old value */
    MATCHUPDATE,
    MATCHNOTCHANGED,
    MATCHCHANGED,
    MATCHINCREASED,
    MATCHDECREASED,
    /* following: compare with both given value and old value */
    MATCHINCREASEDBY,
    MATCHDECREASEDBY
};



/* some routines for working with value_t structures */

/* match_flags: they MUST be implemented as an `uint16_t`, the `__packed__` ensures so.
 * They are reinterpreted as a normal integer when scanning for VLT, which is
 * valid for both endians, as the flags are ordered from smaller to bigger.
 * NAMING: Primitive, single-bit flags are called `flag_*`, while aggregates,
 * defined for convenience, are called `flags_*`*/
enum class flag_t : uint16_t {
    none = 0,

    u8  = 1u<<0u,
    s8  = 1u<<1u,
    u16 = 1u<<2u,
    s16 = 1u<<3u,
    u32 = 1u<<4u,
    s32 = 1u<<5u,
    u64 = 1u<<6u,
    s64 = 1u<<7u,
    f32 = 1u<<8u,
    f64 = 1u<<9u,

    s = s8|s16|s32|s64,   // NOLINT(hicpp-signed-bitwise)
    u = u8|u16|u32|u64,   // NOLINT(hicpp-signed-bitwise)
    i = s|u,
    f = f32|f64,

    i8  = u8 |s8,
    i16 = u16|s16,
    i32 = u32|s32,
    i64 = u64|s64,

    n8  = i8,
    n16 = i16,
    n32 = i32|f32,
    n64 = i64|f64,

    a8  = n8,
    a16 = n8|n16,
    a32 = n8|n16|n32,     // NOLINT(hicpp-signed-bitwise)
    a64 = n8|n16|n32|n64, // NOLINT(hicpp-signed-bitwise)

    max = 0xffffu,
};
BITMASK_DEFINE_VALUE_MASK(flag_t, 0xffff)


class flag : public bitmask::bitmask<flag_t> {
public:
    using bitmask::bitmask;

    explicit constexpr flag(const bitmask<value_type>& flag) noexcept
    : bitmask::bitmask<value_type>(flag) {}

    constexpr static flag_t i_from_size(size_t size) {
        if (size == 8 ) return flag_t::i8;
        if (size == 16) return flag_t::i16;
        if (size == 32) return flag_t::i32;
        if (size == 64) return flag_t::i64;
        return flag_t::none;
    }

    constexpr static flag_t u_from_size(size_t size) {
        if (size == 8 ) return flag_t::u8;
        if (size == 16) return flag_t::u16;
        if (size == 32) return flag_t::u32;
        if (size == 64) return flag_t::u64;
        return flag_t::none;
    }

    constexpr static flag_t s_from_size(size_t size) {
        if (size == 8 ) return flag_t::s8;
        if (size == 16) return flag_t::s16;
        if (size == 32) return flag_t::s32;
        if (size == 64) return flag_t::s64;
        return flag_t::none;
    }

    constexpr static flag_t f_from_size(size_t size) {
        if (size == 32) return flag_t::f32;
        if (size == 64) return flag_t::f64;
        return flag_t::none;
    }

    constexpr static flag_t from_size(size_t size) {
        if (size == 8 ) return flag_t::a8;
        if (size == 16) return flag_t::a16;
        if (size == 32) return flag_t::a32;
        if (size == 64) return flag_t::a64;
        return flag_t::none;
    }

    constexpr size_t size() const {
        return size(*this);
    }

    constexpr static size_t size(flag f) {
        return (f & flag_t::n64) ? 8 :
               (f & flag_t::n32) ? 4 :
               (f & flag_t::n16) ? 2 :
               (f & flag_t::n8 ) ? 1 : 0;
    }

    constexpr operator bool() const { return *this != flag_t::none; }

    std::string str() const {
        flag f{*this};
        if (f == flag_t::none) return "none";
        if (f == flag_t::max) return "max";
        if (f == flag_t::f) return "f";
        if (f == flag_t::i) return "i";

        std::ostringstream os;
        if ((f & flag_t::f64) == flag_t::f64) { os << "f64|"; f &= ~flag_t::f64; }
        if ((f & flag_t::f32) == flag_t::f32) { os << "f32|"; f &= ~flag_t::f32; }
        if ((f & flag_t::u64) == flag_t::u64) { os << "u64|"; f &= ~flag_t::u64; }
        if ((f & flag_t::s64) == flag_t::s64) { os << "s64|"; f &= ~flag_t::s64; }
        if ((f & flag_t::u32) == flag_t::u32) { os << "u32|"; f &= ~flag_t::u32; }
        if ((f & flag_t::s32) == flag_t::s32) { os << "s32|"; f &= ~flag_t::s32; }
        if ((f & flag_t::u16) == flag_t::u16) { os << "u16|"; f &= ~flag_t::u16; }
        if ((f & flag_t::s16) == flag_t::s16) { os << "s16|"; f &= ~flag_t::s16; }
        if ((f & flag_t::u8 ) == flag_t::u8 ) { os << "u8|";  f &= ~flag_t::u8 ; }
        if ((f & flag_t::s8 ) == flag_t::s8 ) { os << "s8|";  f &= ~flag_t::s8 ; }

        if (f != flag_t::none) {
            std::ostringstream().swap(os);
            os << this->bits();
        } else {
            os << "\b";
        }
        return os.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const flag& f0) {
        return os << f0.str();
    }
};


class match {
public:
    uintptr_t address;
    a64 value;
    flag type;

    explicit match()
    : address{0}, value{0}, type{flag_t::none} {}

    explicit match(uintptr_t address, a64 value, flag type)
    : address{address}, value{value}, type{type} {}
};


class user_value {
public:
    struct values {
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
        std::string string;
        flag type;

        size_t int_from_string(const std::string& text) {
            const char *text_c = text.c_str();
            char *endptr;
            char *endptr2;

            /// parse it as signed int
            errno = 0;
            int64_t snum = strtoll(text_c, &endptr, 0);
            bool valid_sint = (errno == 0) && (*endptr == '\0');

            /// parse it as unsigned int
            errno = 0;
            uint64_t unum = strtoull(text_c, &endptr2, 0);
            bool valid_uint = (text[0] != '-') && (errno == 0) && (*endptr2 == '\0');

            if (!valid_sint && !valid_uint)
                return strlen(text_c) - MAX(strlen(endptr), strlen(endptr2));

            /// determine correct flags
            if (valid_sint                                           ) { this->s64 = static_cast<::s64>(snum); this->u64 = static_cast<::u64>(unum); this->type |= flag_t::s64; }
            if (valid_sint && snum >= INT32_MIN && snum <=  INT32_MAX) { this->s32 = static_cast<::s32>(snum); this->u32 = static_cast<::u32>(unum); this->type |= flag_t::s32; }
            if (valid_sint && snum >= INT16_MIN && snum <=  INT16_MAX) { this->s16 = static_cast<::s16>(snum); this->u16 = static_cast<::u16>(unum); this->type |= flag_t::s16; }
            if (valid_sint && snum >=  INT8_MIN && snum <=   INT8_MAX) { this->s8  = static_cast<::s8 >(snum); this->u8  = static_cast<::u8 >(unum); this->type |= flag_t::s8 ; }
            if (valid_uint                                           ) { this->u64 = static_cast<::u64>(unum); this->type |= flag_t::u64; }
            if (valid_uint &&                      unum <= UINT32_MAX) { this->u32 = static_cast<::u32>(unum); this->type |= flag_t::u32; }
            if (valid_uint &&                      unum <= UINT16_MAX) { this->u16 = static_cast<::u16>(unum); this->type |= flag_t::u16; }
            if (valid_uint &&                      unum <=  UINT8_MAX) { this->u8  = static_cast<::u8 >(unum); this->type |= flag_t::u8 ; }
            return strlen(text_c);
        }

        size_t float_from_string(const std::string& text) {
            char *endptr;
            const char *text_c = text.c_str();

            errno = 0;
            double num = strtod(text_c, &endptr);
            if ((errno != 0) || (*endptr != '\0'))
                return strlen(text_c) - strlen(endptr);

            this->type |= flag_t::f;
            this->f32 = static_cast<float>(num);
            this->f64 = num;
            return strlen(text_c);
        }

        size_t from_string(const std::string& text) {
            using namespace std;
            size_t ret;
            if (ret = this->int_from_string(text), ret == text.size()) {
                this->type |= flag_t::f;
                if (this->type & flag_t::s64) {
                    this->f32 = static_cast<float>(this->s64);
                    this->f64 = static_cast<double>(this->s64);
                } else {
                    this->f32 = static_cast<float>(this->u64);
                    this->f64 = static_cast<double>(this->u64);
                }
                return ret;
            } else if (ret = this->float_from_string(text), ret == text.size()) {
                double num = this->f64;
                if (num >= INT64_MIN && num <=  INT64_MAX) { this->s64 = static_cast<::s64>(num); this->u64 = static_cast<::u64>(num); this->type |= flag_t::s64; }
                if (num >= INT32_MIN && num <=  INT32_MAX) { this->s32 = static_cast<::s32>(num); this->u32 = static_cast<::u32>(num); this->type |= flag_t::s32; }
                if (num >= INT16_MIN && num <=  INT16_MAX) { this->s16 = static_cast<::s16>(num); this->u16 = static_cast<::u16>(num); this->type |= flag_t::s16; }
                if (num >=  INT8_MIN && num <=   INT8_MAX) { this->s8  = static_cast<::s8 >(num); this->u8  = static_cast<::u8 >(num); this->type |= flag_t::s8 ; }
                if (num >=         0 && num <= UINT64_MAX) { this->u64 = static_cast<::u64>(num); this->type |= flag_t::u64; }
                if (num >=         0 && num <= UINT32_MAX) { this->u32 = static_cast<::u32>(num); this->type |= flag_t::u32; }
                if (num >=         0 && num <= UINT16_MAX) { this->u16 = static_cast<::u16>(num); this->type |= flag_t::u16; }
                if (num >=         0 && num <=  UINT8_MAX) { this->u8  = static_cast<::u8 >(num); this->type |= flag_t::u8 ; }
                return ret;
            }
            clog<<"parse_uservalue_number(): cannot parse ret: ret: "<<ret<<endl;
            return ret;
        }
    };

    std::vector<values> vars;

    explicit user_value() {}

    explicit user_value(const std::string& s, flag t = flag_t::a64) {
        vars.emplace_back();
        vars[0].from_string(s);
        vars[0].type &= t;
    }
};


NAMESPACE_END(SCANNER)
NAMESPACE_END(PWNER)
