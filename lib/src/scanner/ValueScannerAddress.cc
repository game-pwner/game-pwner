//
// Created by root on 08.05.19.
//

#include <pwner/scanner/ValueScanner.hh>

using namespace PWNER;

// bool
// ScannerAddress::scan_regions(std::vector<std::pair<uintptr_t,uintptr_t>>& writing_matches,
//                              const uintptr_t& from,
//                              const uintptr_t& to) {
//     // todo[med]: uservalue hardcoded to 8 byte
//     step = 4;
//     using namespace std;
//     using namespace std::chrono;
//
//     for(size_t i = 0; i < handler->regions.size(); i++) {
//         char *start = handler->regions_mapped[i].get();
//         char *finish = start + handler->regions[i].size - 7;
//
//         for (char *cursor = start; cursor < finish; cursor += step) {
//             mem64_t *mem = (mem64_t *) cursor;
//             if UNLIKELY(mem->u64 >= from && mem->u64 <= to) {
//                 uintptr_t address = handler->regions[i].address + (cursor - start);
//                 writing_matches.emplace_back(address, mem->u64);
//             }
//         }
//     }
//
//     return true;
// }
//
// bool ScannerAddress::scan_update(std::vector<uintptr_t[2]> &writing_matches) {
//     return false;
// }
//
// bool ScannerAddress::scan_recheck(std::vector<uintptr_t[2]> &writing_matches, const uintptr_t &uservalue) {
//     return false;
// }
//
// bool ScannerAddress::scan_fit(std::vector<uintptr_t[2]> &writing_matches) {
//     return false;
// }
