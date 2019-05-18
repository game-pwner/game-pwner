
#include <pwner/scanner/ValueScanner.hh>


bool
PWNER::ScannerByteSwath::scan_regions(ByteMatches& writing_matches,
                                      const Edata_type& data_type,
                                      const Cuservalue *uservalue,
                                      const Ematch_type& match_type)
{
    using namespace std;
    using namespace std::chrono;

    if (!PWNER::sm_choose_scanroutine(data_type, match_type, uservalue, false)) {
        printf("unsupported scan for current data type.\n");
        return false;
    }

    scan_progress = 0.0;
    stop_flag = false;

    std::vector<PWNER::value_t> res;
    PROCESS::IOCached cachedReader(handler);

    for(const PROCESS::Region& region : handler.regions) {
        size_t region_beg = region.address;
        size_t region_end = region.address + region.size;

        /* For every offset, check if we have a match. */
        size_t required_extra_bytes_to_record = 0;

        for(uintptr_t reg_pos = region_beg; reg_pos < region_end; reg_pos += step) {
            mem64_t memory_ptr{};
            uint64_t copied = cachedReader.read(reg_pos, &memory_ptr, sizeof(mem64_t));
            if UNLIKELY(copied == PROCESS::IO::npos) {
                break;
            }
            PWNER::flag checkflags;

            /* check if we have a match */
            size_t match_length = (*sm_scan_routine)(&memory_ptr, copied, nullptr, uservalue, checkflags);
            if UNLIKELY(match_length > 0) {
                writing_matches.add_element(reg_pos, &memory_ptr, checkflags);
                required_extra_bytes_to_record = match_length - 1;
            } else if UNLIKELY(required_extra_bytes_to_record > 0) {
                writing_matches.add_element(reg_pos, &memory_ptr, PWNER::flag_t::flags_empty);
                required_extra_bytes_to_record--;
            }
        }
    }

    scan_fit(writing_matches);
    scan_progress = 1.0;
    return true;
}

bool
PWNER::ScannerByteSwath::scan_update(PWNER::ByteMatches& writing_matches) {
    PROCESS::IOCached reader(handler);

    // Invalidate cache to get fresh values
    for (ByteSwath& s : writing_matches.swaths) {
        //const size_t copied = (*handler).read(s.base_address, &s.bytes[0], s.bytes.size());
        const size_t copied = reader.read(s.base_address, &s.bytes[0], s.bytes.size());
        /* check if the read succeeded */
        if UNLIKELY(copied == handler.npos) {
            //cout<<"Resizing swath "<<HEX(s.base_address)<<" from "<<s.bytes.size()<<" to "<<0<<" elements"<<endl;
            //cout<<"Error: can not read "<<s.bytes.size()<<" bytes from "<<HEX(s.base_address)<<": "<<strerror(errno)<<endl;
            s.bytes.resize(0);
            s.flags.resize(0);
        } else if UNLIKELY(copied < s.bytes.size()) {
            /* go ahead with the partial read and stop the gathering process */
            s.bytes.resize(copied);
            s.flags.resize(copied);
        }
    }
    scan_fit(writing_matches);
    return true;
}

bool
PWNER::ScannerByteSwath::scan_recheck(ByteMatches& writing_matches,
                                      const PWNER::Edata_type& data_type,
                                      const PWNER::Cuservalue *uservalue,
                                      PWNER::Ematch_type match_type)
{
    using namespace std;

    if (!PWNER::sm_choose_scanroutine(data_type, match_type, uservalue, false)) {
        printf("unsupported scan for current data type.\n");
        return false;
    }

    scan_progress = 0.0;
    stop_flag = false;

    for (ByteSwath& s : writing_matches.swaths) {
        for (size_t it = 0; it < s.bytes.size(); it++) {
            mem64_t *mem = reinterpret_cast<mem64_t *>(&s.bytes[it]);
            PWNER::flag f = s.flags[it];
            size_t mem_size = f.memlength(data_type);

            if (f != flag_t::flags_empty) {
                /* Test only valid old matches */
                value_t val;
                val = s.to_value(it);

                PWNER::flag checkflags; // = flag_t::flags_empty;
                unsigned int match_length = (*sm_scan_routine)(mem, mem_size, &val, uservalue, checkflags);
                s.flags[it] = checkflags;
            }
        }
    }
    scan_fit(writing_matches);

    return true;
}

bool
PWNER::ScannerByteSwath::scan_fit(PWNER::ByteMatches& writing_matches) {
    // Invalidate cache to get fresh values
    for (ByteSwath& s : writing_matches.swaths) {
        s.bytes.shrink_to_fit();
        s.flags.shrink_to_fit();
        assert(s.bytes.capacity() == s.flags.capacity());
        const size_t len = s.flags.size();
        if (len > 6) s.flags[len-7] &= ~flag_t::flags_64b;
        if (len > 5) s.flags[len-6] &= ~flag_t::flags_64b;
        if (len > 4) s.flags[len-5] &= ~flag_t::flags_64b;
        if (len > 3) s.flags[len-4] &= ~flag_t::flags_64b;
        if (len > 2) s.flags[len-3] &= ~flag_t::flags_32b;
        if (len > 1) s.flags[len-2] &= ~flag_t::flags_32b;
        if (len > 0) s.flags[len-1] &= ~flag_t::flags_16b;
    }

    return true;
}
