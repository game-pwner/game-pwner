#pragma once

#include <string>
#include <pwner/common.hh>
#include <pwner/scanner/Value.hh>


NAMESPACE_BEGIN(pwner)

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

    explicit user_value() {}

    explicit user_value(const std::string& s, flag t = flag_t::a64) {
        /*
         * TODO[high]: UI user_value
         * 123 -- value, a64
         * 1.2 -- value, f
         * 123u16 -- value, u16
         * 0x1ABBi64 -- value, i64
         * 12 A0 ?? ?? FF FF -- VLA with wildcards
         * {1.2, 5i64, *, } -- VLA
         * "123" -- string
         * "\xff\x12\x00" -- string
         */
        vars.emplace_back();
        vars[0].from_string(s);
        vars[0].type &= t;
        least = 8; // since it is only one a64 value
    }

public:
    std::vector<values> vars;
    size_t least;
};

NAMESPACE_END(pwner)
