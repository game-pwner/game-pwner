#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <regex>
#include <cmath>
#include <iterator>



namespace sfs = std::filesystem;
// namespace bio = boost::iostreams;

// void foo(std::istream& is, std::ostream& os) {
//     std::string s;
//     while (getline(is, s, '\n')) {
//         os << s;
//     }
// }

class Timer {
public:
    explicit Timer(std::string what = "Timer")
            : m_what(std::move(what)), m_tp(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        std::clog << m_what << ": done in " << std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::high_resolution_clock::now() - m_tp).count() << " seconds" << std::endl;
    }

private:
    std::string m_what;
    std::chrono::high_resolution_clock::time_point m_tp;
};


class Region {
public:
    Region() = default;
    Region(size_t address, size_t size)
            : address(address), size(size) { }

    size_t address;
    size_t size;
};


std::vector<Region> regions;

// Region *
// __attribute__ ((noinline)) get_region__binary_faster(uintptr_t address) {
// }

Region *
__attribute__ ((noinline)) get_region__binary_fast(uintptr_t address) { ///SUCCESS
    std::vector<Region>::iterator first = regions.begin();
    std::vector<Region>::iterator last = regions.end();
    auto mid = last;

    while(first < last) { //todo < or != ?
        mid = first + (std::distance(first, last) / 2);

        if (address < mid->address) {
            last = mid;
        } else if (address >= mid->address + mid->size) {
            first = mid + 1;
        } else {
            return mid.base();
        }
    }
    return nullptr;
}

Region *
__attribute__ ((noinline)) get_region__binary(uintptr_t address) { ///SUCCESS
    size_t first, last, mid;
    first = 0;
    last = regions.size();
    while (first < last) {
        // mid = (first + last) / 2;
        mid = first + (last-first)/2;
        if (address < regions[mid].address) {
            last = mid;
        } else if (address >= regions[mid].address + regions[mid].size) {
            first = mid + 1;
        } else {
            return (Region *)&regions[mid];
        }
    }
    return nullptr;
}

Region *
__attribute__ ((noinline)) get_region__linear(uintptr_t address) {
    for(size_t i = 0; i < regions.size(); i++) {
        if (address >= regions[i].address && address < regions[i].address+regions[i].size)
            return &regions[i];
    }
    return nullptr;
}


// void process_mem_usage(double& vm_usage, double& resident_set)
// {
//     using std::ios_base;
//     using std::ifstream;
//     using std::string;
//
//     vm_usage     = 0.0;
//     resident_set = 0.0;
//
//     // 'file' stat seems to give the most reliable results
//     //
//     ifstream stat_stream("/proc/self/stat",ios_base::in);
//
//     // dummy vars for leading entries in stat that we don't care about
//     //
//     string pid, comm, state, ppid, pgrp, session, tty_nr;
//     string tpgid, flags, minflt, cminflt, majflt, cmajflt;
//     string utime, stime, cutime, cstime, priority, nice;
//     string O, itrealvalue, starttime;
//
//     // the two fields we want
//     //
//     unsigned long vsize;
//     long rss;
//
//     stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
//                 >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
//                 >> utime >> stime >> cutime >> cstime >> priority >> nice
//                 >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
//
//     stat_stream.close();
//
//     long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
//     vm_usage     = vsize / 1024.0;
//     resident_set = rss * page_size_kb;
// }
//


static void oom_score_adj(ssize_t x) {
    std::fstream f("/proc/self/oom_score_adj", std::ios::out);
    if (f.is_open())
        f << std::to_string(x);
}



int main(int argc, char *const argv[], char *envp[]) {
    using namespace std;
    oom_score_adj(999);

#if 0

    // size_t N = 100uLL;
    size_t N = 100000000uLL;
    float a = 3.0f;
    vector<float> x(N);
    vector<float> y(N);
    for (int i = 0; i < N; ++i) {
        x[i] = 2.0f;
        y[i] = 1.0f;
    }

    {
        Timer t("omp      ");

        #pragma omp parallel for
        for (size_t i = 0; i < N; ++i) {
            y[i] = a * x[i] + y[i];
        }
    }
    {
        Timer t("omp, p(a)");

        #pragma omp parallel for firstprivate(a)
        for (size_t i = 0; i < N; ++i) {
            y[i] = a * x[i] + y[i];
        }
    }
    {
        Timer t("acc      ");

        #pragma acc kernels
        for (size_t i = 0; i < N; ++i) {
            y[i] = a * x[i] + y[i];
        }
    }
    {
        Timer t("---------");

        for (size_t i = 0; i < N; ++i) {
            y[i] = a * x[i] + y[i];
        }
    }

    return 1;

#endif

#if 0
    // Bin b1("/home/user/libSegFault.so");               // VM: 35020; RSS: 12816
    // Bin b2("/home/user/libSegFault.so.100000000");     // VM: 327988; RSS: 306028
    // Bin b3("/home/user/libSegFault.so.1000000000");    // VM: 2.9647e+06; RSS: 2.79709e+06
    // Bin b4("/home/user/libSegFault.so.10000000000");   // VM: 35000; RSS: 12944
    // Bin b5("/home/user/libSegFault.so.1000000000000");
    // Bin b6("/home/user/libSegFault.so.10000000000000");

    // double vm, rss;
    // process_mem_usage(vm, rss);
    // cout << "VM: " << vm << "; RSS: " << rss << endl;

    // std::cout << "Hello, World!\n";
    // foo(std::cin, std::cout);


    return 1;
#endif


#if 1
    cout<<showbase;
    const std::vector<size_t> regions_count {0u, 1u, 2u, 3u, 97, 97u, 98u, 99u, 100u, 101u, 102u};
    const uintptr_t start_address = 40;
    uintptr_t end_address = 0;

    for(size_t c : regions_count) {
        srand(0);
        regions.clear();
        end_address = start_address;

        for (size_t i = 0; i < c; i++) {
            uintptr_t size = (rand() % 100)*10; //todo from to
            regions.emplace_back(end_address, size);
            if (i+1 < c)
                end_address += size + (rand()%10);
        }
        // cout<<"start_address: "<<start_address<<endl;
        // cout<<"end_address: "<<end_address<<endl;
        //
        // cout<<"regions: ";
        // for (const auto &r : regions) {
        //     cout<<"["<<r.address<<":"<<r.address+r.size<<"] ";
        // }
        // cout<<"\b"<<endl;


        vector<size_t> needles{
                /* 0   */0,
                /* 1   */ 1,
                /* 2   */ 10,
                /* 3   */ numeric_limits<size_t>::max()/100,
                /* 4   */ numeric_limits<size_t>::max()/2,
                /* 5   */ numeric_limits<size_t>::max(),
                /* 6   */ start_address - 2,
                /* 7   */ start_address - 1,
                /* 8   */ start_address,
                /* 9   */ start_address + 1,
                /* 10  */ start_address + 2,
                /* 11  */ end_address - 2,
                /* 12  */ end_address - 1,
                /* 13  */ end_address,
                /* 14  */ end_address + 1,
                /* 15  */ end_address + 2,
        };
        if (regions.size() >= 1) {
            vector<size_t> needles_ge_1{
                    /* 16 */ regions[0].address - 3,
                    /* 17 */ regions[0].address - 2,
                    /* 18 */ regions[0].address - 1,
                    /* 19 */ regions[0].address,
                    /* 20 */ regions[0].address + 1,
                    /* 21 */ regions[0].address + 2,
                    /* 22 */ regions[0].address + 3,
                    /* 23 */ regions[0].address + regions[0].size - 3,
                    /* 24 */ regions[0].address + regions[0].size - 2,
                    /* 25 */ regions[0].address + regions[0].size - 1,
                    /* 26 */ regions[0].address + regions[0].size,
                    /* 27 */ regions[0].address + regions[0].size + 1,
                    /* 28 */ regions[0].address + regions[0].size + 2,
                    /* 29 */ regions[0].address + regions[0].size + 3,
                    /* 30 */ (regions.end() - 1)->address - 3,
                    /* 31 */ (regions.end() - 1)->address - 2,
                    /* 32 */ (regions.end() - 1)->address - 1,
                    /* 33 */ (regions.end() - 1)->address,
                    /* 34 */ (regions.end() - 1)->address + 1,
                    /* 35 */ (regions.end() - 1)->address + 2,
                    /* 36 */ (regions.end() - 1)->address + 3,
                    /* 37 */ (regions.end() - 1)->address + (regions.end() - 1)->size - 3,
                    /* 38 */ (regions.end() - 1)->address + (regions.end() - 1)->size - 2,
                    /* 39 */ (regions.end() - 1)->address + (regions.end() - 1)->size - 1,
                    /* 40 */ (regions.end() - 1)->address + (regions.end() - 1)->size,
                    /* 41 */ (regions.end() - 1)->address + (regions.end() - 1)->size + 1,
                    /* 42 */ (regions.end() - 1)->address + (regions.end() - 1)->size + 2,
                    /* 43 */ (regions.end() - 1)->address + (regions.end() - 1)->size + 3,
            };
            needles.insert(needles.end(), needles_ge_1.begin(), needles_ge_1.end());
        }
        if (regions.size() >= 2) {
            vector<size_t> needles_ge_2{
                    /* 44  */ regions[1].address - 3,
                    /* 45  */ regions[1].address - 2,
                    /* 46  */ regions[1].address - 1,
                    /* 47  */ regions[1].address,
                    /* 48  */ regions[1].address + 1,
                    /* 49  */ regions[1].address + 2,
                    /* 50  */ regions[1].address + 3,
                    /* 51  */ regions[1].address + regions[1].size - 3,
                    /* 52  */ regions[1].address + regions[1].size - 2,
                    /* 53  */ regions[1].address + regions[1].size - 1,
                    /* 54  */ regions[1].address + regions[1].size,
                    /* 55  */ regions[1].address + regions[1].size + 1,
                    /* 56  */ regions[1].address + regions[1].size + 2,
                    /* 57  */ regions[1].address + regions[1].size + 3,
                    /* 58  */ (regions.end() - 2)->address - 3,
                    /* 59  */ (regions.end() - 2)->address - 2,
                    /* 60  */ (regions.end() - 2)->address - 1,
                    /* 61  */ (regions.end() - 2)->address,
                    /* 62  */ (regions.end() - 2)->address + 1,
                    /* 63  */ (regions.end() - 2)->address + 2,
                    /* 64  */ (regions.end() - 2)->address + 3,
                    /* 65  */ (regions.end() - 2)->address + (regions.end() - 2)->size - 3,
                    /* 66  */ (regions.end() - 2)->address + (regions.end() - 2)->size - 2,
                    /* 67  */ (regions.end() - 2)->address + (regions.end() - 2)->size - 1,
                    /* 68  */ (regions.end() - 2)->address + (regions.end() - 2)->size,
                    /* 69  */ (regions.end() - 2)->address + (regions.end() - 2)->size + 1,
                    /* 70  */ (regions.end() - 2)->address + (regions.end() - 2)->size + 2,
                    /* 71  */ (regions.end() - 2)->address + (regions.end() - 2)->size + 3,
            };
            needles.insert(needles.end(), needles_ge_2.begin(), needles_ge_2.end());
        }
        if (regions.size() >= 3) {
            vector<size_t> needles_ge_3{
                    /* 72  */ regions[2].address - 3,
                    /* 73  */ regions[2].address - 2,
                    /* 74  */ regions[2].address - 1,
                    /* 75  */ regions[2].address,
                    /* 76  */ regions[2].address + 1,
                    /* 77  */ regions[2].address + 2,
                    /* 78  */ regions[2].address + 3,
                    /* 79  */ regions[2].address + regions[2].size - 3,
                    /* 80  */ regions[2].address + regions[2].size - 2,
                    /* 81  */ regions[2].address + regions[2].size - 1,
                    /* 82  */ regions[2].address + regions[2].size,
                    /* 83  */ regions[2].address + regions[2].size + 1,
                    /* 84  */ regions[2].address + regions[2].size + 2,
                    /* 85  */ regions[2].address + regions[2].size + 3,
                    /* 86  */ (regions.end() - 3)->address - 3,
                    /* 87  */ (regions.end() - 3)->address - 2,
                    /* 88  */ (regions.end() - 3)->address - 1,
                    /* 89  */ (regions.end() - 3)->address,
                    /* 90  */ (regions.end() - 3)->address + 1,
                    /* 91  */ (regions.end() - 3)->address + 2,
                    /* 92  */ (regions.end() - 3)->address + 3,
                    /* 93  */ (regions.end() - 3)->address + (regions.end() - 3)->size - 3,
                    /* 94  */ (regions.end() - 3)->address + (regions.end() - 3)->size - 2,
                    /* 95  */ (regions.end() - 3)->address + (regions.end() - 3)->size - 1,
                    /* 96  */ (regions.end() - 3)->address + (regions.end() - 3)->size,
                    /* 97  */ (regions.end() - 3)->address + (regions.end() - 3)->size + 1,
                    /* 98  */ (regions.end() - 3)->address + (regions.end() - 3)->size + 2,
                    /* 99  */ (regions.end() - 3)->address + (regions.end() - 3)->size + 3,
            };
            needles.insert(needles.end(), needles_ge_3.begin(), needles_ge_3.end());
        }
        if (regions.size() >= 4) {
            vector<size_t> needles_ge_4{
                    /* 100 */ regions[3].address - 3,
                    /* 101 */ regions[3].address - 2,
                    /* 102 */ regions[3].address - 1,
                    /* 103 */ regions[3].address,
                    /* 104 */ regions[3].address + 1,
                    /* 105 */ regions[3].address + 2,
                    /* 106 */ regions[3].address + 3,
                    /* 107 */ regions[3].address + regions[3].size - 3,
                    /* 108 */ regions[3].address + regions[3].size - 2,
                    /* 109 */ regions[3].address + regions[3].size - 1,
                    /* 110 */ regions[3].address + regions[3].size,
                    /* 111 */ regions[3].address + regions[3].size + 1,
                    /* 112 */ regions[3].address + regions[3].size + 2,
                    /* 113 */ regions[3].address + regions[3].size + 3,
            };
            needles.insert(needles.end(), needles_ge_4.begin(), needles_ge_4.end());
        }
        for (const size_t& needle : needles) {
            // Region *r1 = get_region__binary(needle); ///success
            Region *r1 = get_region__binary_fast(needle); ///success

            Region *r2 = get_region__linear(needle);
            if (r1 != r2) {
                cout<<"r1 != r2, needle: "<<needle<<" at "<<(&needle-&needles[0])<<", r1: "<<(r1-&regions[0])<<", must be r2: "<<(r2-&regions[0])<<endl;
            }
        }
    }



    // return 1;
#endif

#if 1
    size_t n = 100000000;
//     {
//         Timer t("get_region__binary_faster");
//         #pragma omp parallel for
//         for (size_t i=argc; i<n; i++) {
//             Region *r = get_region__binary_faster(i % end_address);
//             if (r == nullptr)
//                 continue;
//             r->size -= 5;
//             if (r->size <= 0)
//                 r->size += 4;
//         }
//     }
    {
        Timer __t0("===OMP===");

        {
            Timer t("    get_region__binary_fast");
#           pragma omp parallel for
            for (size_t i=argc; i<n; i++) {
                Region *r = get_region__binary_fast(i % end_address);
                if (r == nullptr)
                    continue;
                r->size -= 5;
                if (r->size <= 0)
                    r->size += 4;
            }
        }
        {
            Timer t("    get_region__binary");
#           pragma omp parallel for
            for (size_t i=argc; i<n; i++) {
                Region *r = get_region__binary(i % end_address);
                if (r == nullptr)
                    continue;
                r->size -= 5;
                if (r->size <= 0)
                    r->size += 4;
            }
        }

        {
            Timer t("    get_region__binary_fast");
#           pragma omp parallel for
            for (size_t i=argc; i<n; i++) {
                Region *r = get_region__binary_fast(i % end_address);
                if (r == nullptr)
                    continue;
                r->size -= 5;
                if (r->size <= 0)
                    r->size += 4;
            }
        }
        {
            Timer t("    get_region__binary");
#           pragma omp parallel for
            for (size_t i=argc; i<n; i++) {
                Region *r = get_region__binary(i % end_address);
                if (r == nullptr)
                    continue;
                r->size -= 5;
                if (r->size <= 0)
                    r->size += 4;
            }
        }
//         {
//             Timer t("    get_region__linear");
// #           pragma omp parallel for
//             for (size_t i=argc; i<n; i++) {
//                 Region *r = get_region__linear(i % end_address);
//                 if (r == nullptr)
//                     continue;
//                 r->size -= 5;
//                 if (r->size <= 0)
//                     r->size += 4;
//             }
//         }
    }

    return 0;
#endif
}
