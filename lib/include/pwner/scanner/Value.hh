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


NAMESPACE_BEGIN(pwner)

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

NAMESPACE_END(pwner)
