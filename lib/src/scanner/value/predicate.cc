/*
    Scanning routines for different data types.

    Copyright (C) 2009,2010 WANG Lu <coolwanglu@gmail.com>
    Copyright (C) 2017      Andrea Stacchiotti <andreastacchiotti@gmail.com>

    This file is part of libscanmem.

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

#include <stdbool.h>
#include <pwner/scanner/Value.hh>
#include <pwner/scanner/value/scanner_value.hh>
#include <pwner/common.hh>
#include <functional>


using namespace pwner;


/* todo: try no_inline, maybe we will get some performance */
constexpr void flag_shrink(flag& f, size_t size) {
    if (size <= 7) f &= ~flag_t::n64;
    if (size <= 6) f &= ~flag_t::n64;
    if (size <= 5) f &= ~flag_t::n64;
    if (size <= 4) f &= ~flag_t::n64;
    if (size <= 3) f &= ~flag_t::n32;
    if (size <= 2) f &= ~flag_t::n32;
    if (size <= 1) f &= ~flag_t::n16;
    if (size == 0) f = flag_t::none;
}

template<predicate::variable_predicate MATCH_TYPE, class T>
constexpr auto aksdjakl() {
    if constexpr (MATCH_TYPE == predicate::variable_predicate::EQUAL_TO) return std::equal_to<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::NOT_EQUAL_TO) return std::not_equal_to<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::GRATER_THAN) return std::greater<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::LESS_THAN) return std::less<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::GREATER_EQUAL_THAN) return std::greater_equal<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::LESS_EQUAL_THAN) return std::less_equal<T>();

    if constexpr (MATCH_TYPE == predicate::variable_predicate::CHANGED) return std::not_equal_to<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::NOT_CHANGED) return std::equal_to<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::INCREASED) return std::greater<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::DECREASED) return std::less<T>();

    if constexpr (MATCH_TYPE == predicate::variable_predicate::INCREASED_BY) return std::minus<T>();
    if constexpr (MATCH_TYPE == predicate::variable_predicate::DECREASED_BY) return std::plus<T>();

    // fixme[high]: Нельзя писать static_assert(false) в constexpr if ветке, но можно сделать хак в ютилях
    // template <typename... Args> inline constexpr bool dependent_false = false; и ассертить его как static_assert(dependent_false<T>);
}

template<predicate::variable_predicate MATCH_TYPE, flag_t TYPE_SPEC>
uint16_t scan_routine(const a64 *mem,
                  size_t size,
                  const match *old_match,
                  const user_value& user_value) {
    if constexpr (MATCH_TYPE == predicate::variable_predicate::ANY) {
        flag ret{user_value.vars[0].type};
        if UNLIKELY(size < 8)
            flag_shrink(ret, size);
        return ret;
    }
    if constexpr (MATCH_TYPE == predicate::variable_predicate::EQUAL_TO
               || MATCH_TYPE == predicate::variable_predicate::NOT_EQUAL_TO
               || MATCH_TYPE == predicate::variable_predicate::GRATER_THAN
               || MATCH_TYPE == predicate::variable_predicate::LESS_THAN
               || MATCH_TYPE == predicate::variable_predicate::GREATER_EQUAL_THAN
               || MATCH_TYPE == predicate::variable_predicate::LESS_EQUAL_THAN) {
        flag ret{user_value.vars[0].type};
        if UNLIKELY(size < 8)
            flag_shrink(ret, size);
        if constexpr (TYPE_SPEC & flag_t::u8 ) if ((ret & flag_t::u8 ) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u8 >()(mem->u8 , user_value.vars[0].u8 )) { ret &= ~flag_t::u8 ; }
        if constexpr (TYPE_SPEC & flag_t::u16) if ((ret & flag_t::u16) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u16>()(mem->u16, user_value.vars[0].u16)) { ret &= ~flag_t::u16; }
        if constexpr (TYPE_SPEC & flag_t::u32) if ((ret & flag_t::u32) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u32>()(mem->u32, user_value.vars[0].u32)) { ret &= ~flag_t::u32; }
        if constexpr (TYPE_SPEC & flag_t::u64) if ((ret & flag_t::u64) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u64>()(mem->u64, user_value.vars[0].u64)) { ret &= ~flag_t::u64; }
        if constexpr (TYPE_SPEC & flag_t::s8 ) if ((ret & flag_t::s8 ) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s8 >()(mem->s8 , user_value.vars[0].s8 )) { ret &= ~flag_t::s8 ; }
        if constexpr (TYPE_SPEC & flag_t::s16) if ((ret & flag_t::s16) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s16>()(mem->s16, user_value.vars[0].s16)) { ret &= ~flag_t::s16; }
        if constexpr (TYPE_SPEC & flag_t::s32) if ((ret & flag_t::s32) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s32>()(mem->s32, user_value.vars[0].s32)) { ret &= ~flag_t::s32; }
        if constexpr (TYPE_SPEC & flag_t::s64) if ((ret & flag_t::s64) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s64>()(mem->s64, user_value.vars[0].s64)) { ret &= ~flag_t::s64; }
        if constexpr (TYPE_SPEC & flag_t::f32) if ((ret & flag_t::f32) == flag_t::none || !aksdjakl<MATCH_TYPE, ::f32>()(mem->f32, user_value.vars[0].f32)) { ret &= ~flag_t::f32; }
        if constexpr (TYPE_SPEC & flag_t::f64) if ((ret & flag_t::f64) == flag_t::none || !aksdjakl<MATCH_TYPE, ::f64>()(mem->f64, user_value.vars[0].f64)) { ret &= ~flag_t::f64; }
        return ret;
    }
    if constexpr (MATCH_TYPE == predicate::variable_predicate::CHANGED
               || MATCH_TYPE == predicate::variable_predicate::NOT_CHANGED
               || MATCH_TYPE == predicate::variable_predicate::INCREASED
               || MATCH_TYPE == predicate::variable_predicate::DECREASED) {
        flag ret{static_cast<flag_t>(old_match->type)};
        if UNLIKELY(size < 8)
            flag_shrink(ret, size);
        if constexpr (TYPE_SPEC & flag_t::u8 ) if ((ret & flag_t::u8 ) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u8 >()(mem->u8 , old_match->value.u8 )) { ret &= ~flag_t::u8 ; }
        if constexpr (TYPE_SPEC & flag_t::u16) if ((ret & flag_t::u16) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u16>()(mem->u16, old_match->value.u16)) { ret &= ~flag_t::u16; }
        if constexpr (TYPE_SPEC & flag_t::u32) if ((ret & flag_t::u32) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u32>()(mem->u32, old_match->value.u32)) { ret &= ~flag_t::u32; }
        if constexpr (TYPE_SPEC & flag_t::u64) if ((ret & flag_t::u64) == flag_t::none || !aksdjakl<MATCH_TYPE, ::u64>()(mem->u64, old_match->value.u64)) { ret &= ~flag_t::u64; }
        if constexpr (TYPE_SPEC & flag_t::s8 ) if ((ret & flag_t::s8 ) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s8 >()(mem->s8 , old_match->value.s8 )) { ret &= ~flag_t::s8 ; }
        if constexpr (TYPE_SPEC & flag_t::s16) if ((ret & flag_t::s16) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s16>()(mem->s16, old_match->value.s16)) { ret &= ~flag_t::s16; }
        if constexpr (TYPE_SPEC & flag_t::s32) if ((ret & flag_t::s32) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s32>()(mem->s32, old_match->value.s32)) { ret &= ~flag_t::s32; }
        if constexpr (TYPE_SPEC & flag_t::s64) if ((ret & flag_t::s64) == flag_t::none || !aksdjakl<MATCH_TYPE, ::s64>()(mem->s64, old_match->value.s64)) { ret &= ~flag_t::s64; }
        if constexpr (TYPE_SPEC & flag_t::f32) if ((ret & flag_t::f32) == flag_t::none || !aksdjakl<MATCH_TYPE, ::f32>()(mem->f32, old_match->value.f32)) { ret &= ~flag_t::f32; }
        if constexpr (TYPE_SPEC & flag_t::f64) if ((ret & flag_t::f64) == flag_t::none || !aksdjakl<MATCH_TYPE, ::f64>()(mem->f64, old_match->value.f64)) { ret &= ~flag_t::f64; }
        return ret;
    }
    if constexpr (MATCH_TYPE == predicate::variable_predicate::INCREASED_BY
               || MATCH_TYPE == predicate::variable_predicate::DECREASED_BY) {
        flag ret{static_cast<flag_t>(old_match->type)};
        if UNLIKELY(size < 8)
            flag_shrink(ret, size);
        if constexpr (TYPE_SPEC & flag_t::u8 ) if ((ret & flag_t::u8 ) == flag_t::none || mem->u8  != aksdjakl<MATCH_TYPE, ::u8 >()(mem->u8 , old_match->value.u8 )) { ret &= ~flag_t::u8 ; }
        if constexpr (TYPE_SPEC & flag_t::u16) if ((ret & flag_t::u16) == flag_t::none || mem->u16 != aksdjakl<MATCH_TYPE, ::u16>()(mem->u16, old_match->value.u16)) { ret &= ~flag_t::u16; }
        if constexpr (TYPE_SPEC & flag_t::u32) if ((ret & flag_t::u32) == flag_t::none || mem->u32 != aksdjakl<MATCH_TYPE, ::u32>()(mem->u32, old_match->value.u32)) { ret &= ~flag_t::u32; }
        if constexpr (TYPE_SPEC & flag_t::u64) if ((ret & flag_t::u64) == flag_t::none || mem->u64 != aksdjakl<MATCH_TYPE, ::u64>()(mem->u64, old_match->value.u64)) { ret &= ~flag_t::u64; }
        if constexpr (TYPE_SPEC & flag_t::s8 ) if ((ret & flag_t::s8 ) == flag_t::none || mem->s8  != aksdjakl<MATCH_TYPE, ::s8 >()(mem->s8 , old_match->value.s8 )) { ret &= ~flag_t::s8 ; }
        if constexpr (TYPE_SPEC & flag_t::s16) if ((ret & flag_t::s16) == flag_t::none || mem->s16 != aksdjakl<MATCH_TYPE, ::s16>()(mem->s16, old_match->value.s16)) { ret &= ~flag_t::s16; }
        if constexpr (TYPE_SPEC & flag_t::s32) if ((ret & flag_t::s32) == flag_t::none || mem->s32 != aksdjakl<MATCH_TYPE, ::s32>()(mem->s32, old_match->value.s32)) { ret &= ~flag_t::s32; }
        if constexpr (TYPE_SPEC & flag_t::s64) if ((ret & flag_t::s64) == flag_t::none || mem->s64 != aksdjakl<MATCH_TYPE, ::s64>()(mem->s64, old_match->value.s64)) { ret &= ~flag_t::s64; }
        if constexpr (TYPE_SPEC & flag_t::f32) if ((ret & flag_t::f32) == flag_t::none || mem->f32 != aksdjakl<MATCH_TYPE, ::f32>()(mem->f32, old_match->value.f32)) { ret &= ~flag_t::f32; }
        if constexpr (TYPE_SPEC & flag_t::f64) if ((ret & flag_t::f64) == flag_t::none || mem->f64 != aksdjakl<MATCH_TYPE, ::f64>()(mem->f64, old_match->value.f64)) { ret &= ~flag_t::f64; }
        return ret;
    }
    // if constexpr (MATCHTYPE == match_type_t::MATCHRANGE) {
    //     flag ret{flag_t::none};
    //     if (user_value.vars[0].type & flag::s_from_size(DATAWIDTH) && s_asdasd<DATAWIDTH, std::less_equal>(mem, old_match)) { ret |= flag::s_from_size(DATAWIDTH); }
    //     //...
    //     return ret;
    // }
    return flag{flag_t::none};
}

template<flag_t TYPE_SPEC>
uint16_t vla_predicate(const a64 *mem,
                   size_t size,
                   const match *old_match,
                   const user_value& user_value) {
    if constexpr (TYPE_SPEC == flag_t::u8)
        ;
    return 0;
}

template<flag_t TYPE_SPEC>
uint16_t string_predicate(const a64 *mem,
                      size_t size,
                      const match *old_match,
                      const user_value& user_value) {
    if constexpr (TYPE_SPEC == flag_t::u8)
        ;
    return 0;
}


template <predicate::variable_predicate MATCH_TYPE>
predicate::predicate_t get_variable_predicate_(const user_value& user_value) {
    if (user_value.vars[0].type == flag_t::i8 ) return scan_routine<MATCH_TYPE, flag_t::i8 >;
    if (user_value.vars[0].type == flag_t::i16) return scan_routine<MATCH_TYPE, flag_t::i16>;
    if (user_value.vars[0].type == flag_t::i32) return scan_routine<MATCH_TYPE, flag_t::i32>;
    if (user_value.vars[0].type == flag_t::i64) return scan_routine<MATCH_TYPE, flag_t::i64>;
    if (user_value.vars[0].type == flag_t::f32) return scan_routine<MATCH_TYPE, flag_t::f32>;
    if (user_value.vars[0].type == flag_t::f64) return scan_routine<MATCH_TYPE, flag_t::f64>;
    if (user_value.vars[0].type == flag_t::n32) return scan_routine<MATCH_TYPE, flag_t::n32>;
    if (user_value.vars[0].type == flag_t::n64) return scan_routine<MATCH_TYPE, flag_t::n64>;
    return scan_routine<MATCH_TYPE, flag_t::a64>;
}

predicate::predicate_t pwner::predicate::get_variable_predicate(const user_value& user_value, predicate::variable_predicate e) {
    if (e == predicate::variable_predicate::ANY)                return get_variable_predicate_<predicate::variable_predicate::ANY>(user_value);
    if (e == predicate::variable_predicate::EQUAL_TO)           return get_variable_predicate_<predicate::variable_predicate::EQUAL_TO>(user_value);
    if (e == predicate::variable_predicate::NOT_EQUAL_TO)       return get_variable_predicate_<predicate::variable_predicate::NOT_EQUAL_TO>(user_value);
    if (e == predicate::variable_predicate::GRATER_THAN)        return get_variable_predicate_<predicate::variable_predicate::GRATER_THAN>(user_value);
    if (e == predicate::variable_predicate::LESS_THAN)          return get_variable_predicate_<predicate::variable_predicate::LESS_THAN>(user_value);
    if (e == predicate::variable_predicate::GREATER_EQUAL_THAN) return get_variable_predicate_<predicate::variable_predicate::GREATER_EQUAL_THAN>(user_value);
    if (e == predicate::variable_predicate::LESS_EQUAL_THAN)    return get_variable_predicate_<predicate::variable_predicate::LESS_EQUAL_THAN>(user_value);
    if (e == predicate::variable_predicate::IN_RANGE)           return get_variable_predicate_<predicate::variable_predicate::IN_RANGE>(user_value);
    if (e == predicate::variable_predicate::NOT_IN_RANGE)       return get_variable_predicate_<predicate::variable_predicate::NOT_IN_RANGE>(user_value);
    if (e == predicate::variable_predicate::CHANGED)            return get_variable_predicate_<predicate::variable_predicate::CHANGED>(user_value);
    if (e == predicate::variable_predicate::NOT_CHANGED)        return get_variable_predicate_<predicate::variable_predicate::NOT_CHANGED>(user_value);
    if (e == predicate::variable_predicate::INCREASED)          return get_variable_predicate_<predicate::variable_predicate::INCREASED>(user_value);
    if (e == predicate::variable_predicate::DECREASED)          return get_variable_predicate_<predicate::variable_predicate::DECREASED>(user_value);
    if (e == predicate::variable_predicate::INCREASED_BY)       return get_variable_predicate_<predicate::variable_predicate::INCREASED_BY>(user_value);
    if (e == predicate::variable_predicate::DECREASED_BY)       return get_variable_predicate_<predicate::variable_predicate::DECREASED_BY>(user_value);
    return nullptr;
}

predicate::predicate_t pwner::predicate::get_vla_predicate(const user_value& user_value) {
    return vla_predicate<flag_t::i8>;
}

predicate::predicate_t pwner::predicate::get_string_predicate(const user_value& user_value) {
    return string_predicate<flag_t::i8>;
}
