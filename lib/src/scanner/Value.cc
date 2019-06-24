/*
    This file is part of Reverse Engine.

    

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

#include <pwner/scanner/Value.hh>






// void
// PWNER::string_to_uservalue(const PWNER::data_type_t &data_type,
//                            const std::string &text,
//                            PWNER::match_type_t &match_type,
//                            std::vector<PWNER::Cuservalue>& uservalue)
// {
//     //todo[medium] remove GOTO!!!!!!!!
//     using namespace std;
//     uservalue.resize(2);
//
//     if (text.empty())
//         throw bad_uservalue_cast(text,0);
//
//     switch (data_type) {
//         case PWNER::data_type_t::BYTEARRAY:
// //            if (!parse_uservalue_bytearray(text, &uservalue[0])) {
// //                return false;
// //            }
//             return;
//         case PWNER::data_type_t::STRING:
//             uservalue[0].string_value = const_cast<char *>(text.c_str());
//             uservalue[0].flags = flag_t::flags_max;
//             return;
//         default:
//             // ╔════╤════════════╤═══════╤═══════════════════════════╤═════════════════════════════════════╗
//             // ║    │ C operator │ Alias │ Description               │ Description (if no number provided) ║
//             // ╠════╪════════════╪═══════╪═══════════════════════════╪═════════════════════════════════════╣
//             // ║ a1 │            │ ?     │ unknown initial value     │                                     ║
//             // ║ a2 │ ++         │       │ increased                 │                                     ║
//             // ║ a3 │ --         │       │ decreased                 │                                     ║
//             // ╟────┼────────────┼───────┼───────────────────────────┼─────────────────────────────────────╢
//             // ║ b1 │ == N       │ N     │ exactly N                 │ not changed                         ║
//             // ║ b2 │ != N       │       │ not N                     │ changed                             ║
//             // ║ b3 │ += N       │       │ increased by N            │ increased                           ║
//             // ║ b4 │ -= N       │       │ decreased by N            │ decreased                           ║
//             // ║ b5 │ > N        │       │ greater than N            │ increased (just for ux)             ║
//             // ║ b6 │ < N        │       │ less than N               │ decreased (just for ux)             ║
//             // ╟────┼────────────┼───────┼───────────────────────────┼─────────────────────────────────────╢
//             // ║ d  │            │ N..M  │ range                     │                                     ║
//             // ╚════╧════════════╧═══════╧═══════════════════════════╧═════════════════════════════════════╝
//             string pattern(text.size(), '\0');
//
//             //remove all spaces
//             auto it = copy_if(text.begin(),
//                               text.end(),
//                               pattern.begin(),
//                               [](int i) { return (!isspace(i) && i != '_' && i != '\''); });
//             pattern.resize(static_cast<size_t>(distance(pattern.begin(), it)));
//
//             auto boilerplate_a = [&](const string &MASK, PWNER::match_type_t MATCH_TYPE_NO_VALUE) -> bool
//             {
//                 size_t cursor = pattern.find(MASK);
//                 if (cursor == string::npos)  // if mask not found
//                     return false;
//                 cursor += MASK.size();
//                 if (cursor != pattern.size())  // if extra symbols after
//                     throw bad_uservalue_cast(pattern, cursor);
//                 match_type = MATCH_TYPE_NO_VALUE;
//                 return true;
//             };
//             if (boilerplate_a("?", PWNER::match_type_t::MATCHANY))        goto valid_number;  // a1
//             if (boilerplate_a("++", PWNER::match_type_t::MATCHINCREASED)) goto valid_number;  // a2
//             if (boilerplate_a("--", PWNER::match_type_t::MATCHDECREASED)) goto valid_number;  // a3
//
//             auto boilerplate_b = [&](const string &MASK, PWNER::match_type_t MATCH_TYPE, PWNER::match_type_t MATCH_TYPE_NO_VALUE) -> bool
//             {
//                 size_t cursor = pattern.find(MASK);
//                 if (cursor == string::npos)  // if mask not found
//                     return false;
//                 cursor += MASK.size();
//                 if (cursor == pattern.size()) {  // end reached
//                     match_type = MATCH_TYPE_NO_VALUE;
//                     return true;
//                 }
//                 const string &possible_number = pattern.substr(cursor);  // slice [cursor:-1]
//                 size_t pos = uservalue[0].parse_uservalue_number(possible_number);
//                 if (pos != possible_number.size())  // if parse_uservalue_number() not parsed whole possible_number
//                     throw bad_uservalue_cast(pattern, cursor + pos);
//                 match_type = MATCH_TYPE;
//                 return true;
//             };
//             if (boilerplate_b("==", PWNER::match_type_t::MATCHEQUALTO,     PWNER::match_type_t::MATCHNOTCHANGED)) goto valid_number; // 1
//             if (boilerplate_b("!=", PWNER::match_type_t::MATCHNOTEQUALTO,  PWNER::match_type_t::MATCHCHANGED))    goto valid_number; // 2
//             if (boilerplate_b("+=", PWNER::match_type_t::MATCHINCREASEDBY, PWNER::match_type_t::MATCHINCREASED))  goto valid_number; // 3
//             if (boilerplate_b("-=", PWNER::match_type_t::MATCHDECREASEDBY, PWNER::match_type_t::MATCHDECREASED))  goto valid_number; // 4
//             if (boilerplate_b(">",  PWNER::match_type_t::MATCHGREATERTHAN, PWNER::match_type_t::MATCHINCREASED))  goto valid_number; // 5
//             if (boilerplate_b("<",  PWNER::match_type_t::MATCHLESSTHAN,    PWNER::match_type_t::MATCHDECREASED))  goto valid_number; // 6
//
//
//             // todo[low] MATCHNOTINRANGE "!= 0..1", MATCHNOTINRANGE / MATCHEXCLUDE
//             auto boilerplate_c = [&](const string &MASK, PWNER::match_type_t MATCH_TYPE) -> bool
//             {
//                 size_t cursor = pattern.find(MASK);
//                 if (cursor == string::npos)   // if mask not found
//                     return false;
//
//                 /// Parse first number N..M
//                 ///                    ^
//                 const string &possible_number = pattern.substr(0, cursor-0);  // slice [0:cursor]
//                 clog<<"pattern.substr(0, cursor): \""<<pattern.substr(0, cursor)<<"\""<<endl;
//                 clog<<"pattern.substr(0, cursor+1): \""<<pattern.substr(0, cursor+1)<<"\""<<endl;
//                 size_t pos = uservalue[0].parse_uservalue_number(possible_number);
//                 if (pos != possible_number.size())  // if parse_uservalue_number() not parsed whole possible_number
//                     throw bad_uservalue_cast(pattern, 0+pos);
//                 cursor += MASK.size();
//
//                 /// Parse last number N..M
//                 ///                      ^
//                 const string &possible_number2 = pattern.substr(cursor);  // slice [cursor:-1]
//                 size_t pos2 = uservalue[1].parse_uservalue_number(possible_number2);
//                 if (pos2 != possible_number2.size())
//                     throw bad_uservalue_cast(pattern, cursor+pos2);
//
//                 /// Check that the range is nonempty
//                 if (uservalue[0].f64 > uservalue[1].f64) {
//                     clog<<"Empty range"<<endl;
//                     throw bad_uservalue_cast(pattern, cursor);
//                 }
//                 /// Store the bitwise AND of both flags in the first value
//                 uservalue[0].flags &= uservalue[1].flags;
//                 match_type = MATCH_TYPE;
//                 return true;
//             };
//             if (boilerplate_c("..",  PWNER::match_type_t::MATCHRANGE)) goto valid_number;
//
//             // ==
//             {
//                 size_t pos = uservalue[0].parse_uservalue_number(pattern);
//                 if (pos != pattern.size()) { // if parse_uservalue_number() not parsed whole possible_number
//                     throw bad_uservalue_cast(pattern, pos);
//                 }
//                 match_type = PWNER::match_type_t::MATCHEQUALTO;
//             }
//     }
//
//     valid_number:;
// //fixme[high]: Edata_type_to_match_flags must be a function
//     uservalue[0].flags &= PWNER::flag(data_type);
//     uservalue[1].flags &= PWNER::flag(data_type);
// }
//

/// https://github.com/scanmem/scanmem/blob/master/sets.c
// size_t PWNER::Cuservalue::parse_uservalue_int(const std::string& text)
// {
//     const char *text_c = text.c_str();
//     char *endptr;
//     char *endptr2;
//
//     /// parse it as signed int
//     errno = 0;
//     int64_t snum = strtoll(text_c, &endptr, 0);
//     bool valid_sint = (errno == 0) && (*endptr == '\0');
//
//     /// parse it as unsigned int
//     errno = 0;
//     uint64_t unum = strtoull(text_c, &endptr2, 0);
//     bool valid_uint = (text[0] != '-') && (errno == 0) && (*endptr2 == '\0');
//
//     if (!valid_sint && !valid_uint)
//         return strlen(text_c) - MAX(strlen(endptr), strlen(endptr2));
//
//     /// determine correct flags
//     if (valid_sint                                           ) { this->i64 = static_cast< int64_t>(snum); this->flags |= flag_t::flag_i64; }
//     if (valid_sint && snum >= INT32_MIN && snum <=  INT32_MAX) { this->i32 = static_cast< int32_t>(snum); this->flags |= flag_t::flag_i32; }
//     if (valid_sint && snum >= INT16_MIN && snum <=  INT16_MAX) { this->i16 = static_cast< int16_t>(snum); this->flags |= flag_t::flag_i16; }
//     if (valid_sint && snum >=  INT8_MIN && snum <=   INT8_MAX) { this->i8  = static_cast< int8_t >(snum); this->flags |= flag_t::flag_i8 ; }
//     if (valid_uint                                           ) { this->u64 = static_cast<uint64_t>(unum); this->flags |= flag_t::flag_u64; }
//     if (valid_uint &&                      unum <= UINT32_MAX) { this->u32 = static_cast<uint32_t>(unum); this->flags |= flag_t::flag_u32; }
//     if (valid_uint &&                      unum <= UINT16_MAX) { this->u16 = static_cast<uint16_t>(unum); this->flags |= flag_t::flag_u16; }
//     if (valid_uint &&                      unum <=  UINT8_MAX) { this->u8  = static_cast<uint8_t >(unum); this->flags |= flag_t::flag_u8 ; }
//
//     /// If signed flag exist and unsigned not exist, fill unsigned value. Used at first_scan() to scan 5% faster.
//     if ((this->flags & flag_t::flag_i64) && !(this->flags & flag_t::flag_u64)) { this->u64 = static_cast<uint64_t>(this->i64); }
//     if ((this->flags & flag_t::flag_i32) && !(this->flags & flag_t::flag_u32)) { this->u32 = static_cast<uint32_t>(this->i32); }
//     if ((this->flags & flag_t::flag_i16) && !(this->flags & flag_t::flag_u16)) { this->u16 = static_cast<uint16_t>(this->i16); }
//     if ((this->flags & flag_t::flag_i8 ) && !(this->flags & flag_t::flag_u8 )) { this->u8  = static_cast<uint8_t >(this->i8 ); }
//     return strlen(text_c);
// }

// fixme[med] this shit parses float only with comma, dot won't parse
// fixme[med] also can't parse "nan", "inf" and "-inf"
// double a = -1./0.;
// double b = 0./0. ;
// double c = 1./0. ;
// clog<<"-1./0.  = "<<a<<" = "<<hex<<*reinterpret_cast<uint64_t *>(&a)<<dec<<endl;
// clog<<"0./0.   = "<<b<<" = "<<hex<<*reinterpret_cast<uint64_t *>(&b)<<dec<<endl;
// clog<<"1./0.   = "<<c<<" = "<<hex<<*reinterpret_cast<uint64_t *>(&c)<<dec<<endl;
// size_t PWNER::Cuservalue::parse_uservalue_float(const std::string& text)
// {
//     char *endptr;
//     const char *text_c = text.c_str();
//
//     errno = 0;
//     double num = strtod(text_c, &endptr);
//     if ((errno != 0) || (*endptr != '\0'))
//         return strlen(text_c) - strlen(endptr);
//
//     this->flags |= flag_t::flags_float;
//     this->f32 = static_cast<float>(num);
//     this->f64 = num;
//     return strlen(text_c);
// }
//
// size_t PWNER::Cuservalue::parse_uservalue_number(const std::string& text)
// {
//     using namespace std;
//     size_t ret;
//     if ((ret = this->parse_uservalue_int(text)) && ret == text.size()) {
//         this->flags |= flag_t::flags_float;
//         if (this->flags & flag_t::flag_i64) {
//             this->f32 = static_cast<float>(this->i64);
//             this->f64 = static_cast<double>(this->i64);
//         } else {
//             this->f32 = static_cast<float>(this->u64);
//             this->f64 = static_cast<double>(this->u64);
//         }
//         return ret;
//     } else if ((ret = this->parse_uservalue_float(text)) && ret == text.size()) {
//         double num = this->f64;
//         if (num >= INT64_MIN && num <=  INT64_MAX) { this->i64 = static_cast< int64_t>(num); this->flags |= flag_t::flag_i64; }
//         if (num >= INT32_MIN && num <=  INT32_MAX) { this->i32 = static_cast< int32_t>(num); this->flags |= flag_t::flag_i32; }
//         if (num >= INT16_MIN && num <=  INT16_MAX) { this->i16 = static_cast< int16_t>(num); this->flags |= flag_t::flag_i16; }
//         if (num >=  INT8_MIN && num <=   INT8_MAX) { this->i8  = static_cast< int8_t >(num); this->flags |= flag_t::flag_i8 ; }
//         if (num >=         0 && num <= UINT64_MAX) { this->u64 = static_cast<uint64_t>(num); this->flags |= flag_t::flag_u64; }
//         if (num >=         0 && num <= UINT32_MAX) { this->u32 = static_cast<uint32_t>(num); this->flags |= flag_t::flag_u32; }
//         if (num >=         0 && num <= UINT16_MAX) { this->u16 = static_cast<uint16_t>(num); this->flags |= flag_t::flag_u16; }
//         if (num >=         0 && num <=  UINT8_MAX) { this->u8  = static_cast<uint8_t >(num); this->flags |= flag_t::flag_u8 ; }
//
//         /// If signed flag exist and unsigned not exist, fill unsigned value. Used at first_scan() to scan 5% faster.
//         if ((this->flags & flag_t::flag_i64) && !(this->flags & flag_t::flag_u64)) { this->u64 = static_cast<uint64_t>(this->i64); }
//         if ((this->flags & flag_t::flag_i32) && !(this->flags & flag_t::flag_u32)) { this->u32 = static_cast<uint32_t>(this->i32); }
//         if ((this->flags & flag_t::flag_i16) && !(this->flags & flag_t::flag_u16)) { this->u16 = static_cast<uint16_t>(this->i16); }
//         if ((this->flags & flag_t::flag_i8 ) && !(this->flags & flag_t::flag_u8 )) { this->u8  = static_cast<uint8_t >(this->i8 ); }
//         return ret;
//     }
//     clog<<"parse_uservalue_number(): cannot parse ret: ret: "<<ret<<endl;
//     return ret;
// }

/* parse bytearray, it will allocate the arrays itself, then needs to be free'd by `free_uservalue()` */
//TODO create entry for mask, not only for pattern
//FIXME[high] UNDONE
// size_t PWNER::Cuservalue::parse_uservalue_bytearray(const std::string& text_s)
// {
//     using namespace std;
//     vector<uint8_t> bytes_array;
//     vector<PWNER::wildcard_t> wildcards_array;
//
//     /// @see https://stackoverflow.com/questions/7397768/choice-between-vectorresize-and-vectorreserve
//     // fixme[low]: hardcode
//     bytes_array.reserve(32);
//     wildcards_array.reserve(32);
//
//     /// skip past any whitespace
//     char *text = const_cast<char *>(&text_s[0]);
//     while (isspace(*text))
//         ++text;
//
//     /// split text by whitespace and(or) \x-like sequence "\xDE\xAD" (even "x\DExAD", sorry xD) and "BE EF"
//     char *cur_str;
//     cur_str = strtok(const_cast<char *>(text), R"( \x)");
//
//     for(; *cur_str != '\0';) { //fixme должно же работать, не?
//         /// test its length
//         if (strlen(cur_str) != 2)
//             return false;
//
//         /// if unknown value
//         if ((strcmp(cur_str, "??") == 0)) {
//             bytes_array.push_back(0x00);
//             wildcards_array.push_back(WILDCARD);
//         } else {
//             /// parse as hex integer
//             char *endptr;
//             uint8_t cur_byte = static_cast<uint8_t>(strtoul(cur_str, &endptr, 16));
//             if (*endptr != '\0')
//                 return false;
//
//             bytes_array.push_back(cur_byte);
//             wildcards_array.push_back(FIXED);
//         }
//
//         /// get next byte
//         cur_str = strtok(NULL, R"( \x)");
//     }
//
//     /* everything is ok */
//     this->wildcard_value = wildcards_array.data();
//     this->bytearray_value = bytes_array.data();
//     this->flags = flag_t::flags_all;
//     return true;
// }
//
// //FIXME[high]: to revision
// size_t PWNER::Cuservalue::parse_uservalue_string(const std::string& text_s)
// {
//     using namespace std;
//     vector<uint8_t> bytes_array;
//     vector<PWNER::wildcard_t> wildcards_array;
//
//     /// @see https://stackoverflow.com/questions/7397768/choice-between-vectorresize-and-vectorreserve
//     // fixme[low]: hardcode
//     bytes_array.reserve(32);
//     wildcards_array.reserve(32);
//
//     /// skip past any whitespace
//     char *text = const_cast<char *>(&text_s[0]);
//     while (isspace(*text))
//         ++text;
//
//     /// split text by whitespace and(or) \x-like sequence "\xDE\xAD" and "BE EF" (and "x\DExAD", sorry xD)
//     char *cur_str;
//     cur_str = strtok(const_cast<char *>(text), R"( \x)");
//
//     for(; *cur_str != '\0';) { //fixme должно же работать, не?
//         /// test its length
//         if (strlen(cur_str) != 2)
//             return false;
//
//         /// if unknown value
//         if ((strcmp(cur_str, "??") == 0)) {
//             bytes_array.push_back(0x00);
//             wildcards_array.push_back(PWNER::WILDCARD);
//         } else {
//             /// parse as hex integer
//             char *endptr;
//             uint8_t cur_byte = static_cast<uint8_t>(strtoul(cur_str, &endptr, 16));
//             if (*endptr != '\0')
//                 return false;
//
//             bytes_array.push_back(cur_byte);
//             wildcards_array.push_back(PWNER::FIXED);
//         }
//
//         /// get next byte
//         cur_str = strtok(NULL, R"( \x)");
//     }
//
//     /* everything is ok */
//     this->wildcard_value = wildcards_array.data();
//     this->bytearray_value = bytes_array.data();
//     this->flags = flag_t::flags_all;
//     return true;
// }
