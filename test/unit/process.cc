#include <boost/test/auto_unit_test.hpp>
#include <pwner/process/Process.hh>
#include <ostream>
#include <cmath>


BOOST_AUTO_TEST_SUITE(process)

class mem_t {
public:
    mem_t(const uint64_t& i, const double& fp) {
        _u8  = static_cast<uint8_t >(i);
        _u16 = static_cast<uint16_t>(i);
        _u32 = static_cast<uint32_t>(i);
        _u64 = static_cast<uint64_t>(i);

        _f32 = static_cast<float>(fp);
        _f64 = static_cast<double>(fp);

        _cs = "this is std::string с юникодом";
        _ws = L"this is std::windows_string с юникодом";
        _pc = "l33t";
        _pc_reserved = "\x00""1337 ways to write digits after hex value inside c string...";
    }

    ~mem_t() = default;

public:
    uint8_t _u8;
    uint64_t _u16;
    uint32_t _u32;
    uint64_t _u64;

    float _f32;
    double _f64;

    std::string _cs;
    std::wstring _ws;
    const char *_pc;
    const char *_pc_reserved;
};


BOOST_AUTO_TEST_CASE(ProcessProcfs_functional)
{
    using namespace std::string_literals;

    int argc = boost::unit_test::framework::master_test_suite().argc;
    char **argv = boost::unit_test::framework::master_test_suite().argv;
    std::filesystem::path p(argv[0]);

    std::string regex_special_initializer = R"([-[\]{}()*+?.,\^$|#\s])";
    std::regex regex_special{regex_special_initializer};
    // BOOST_REQUIRE_EQUAL(std::regex_replace(regex_special_initializer, regex_special, R"(\$&)"), R"([\[\-\[\\]\{\}\(\)\*\+\?\.\,\\^\$\|\#\s\])"s);

    std::string cmdline = std::regex_replace(argv[0], regex_special, R"(\$&)");
    for (char **_argv = argv + 1; _argv < argv + argc; _argv++)
        cmdline += ".+" + std::regex_replace(*_argv, regex_special, R"(\$&)");

    PWNER::PROCESS::ProcessProcfs proc(getpid());
    BOOST_REQUIRE(proc);
    BOOST_CHECK(proc.running());
    BOOST_CHECK_EQUAL(proc.cwd(), std::filesystem::current_path());
    BOOST_CHECK_EQUAL(proc.exe(), p);
    BOOST_CHECK(std::regex_search(proc.cmdline(), std::regex(cmdline)));
    BOOST_CHECK_EQUAL(proc.pid(), getpid());
    BOOST_WARN_EQUAL(PWNER::PROCESS::ProcessProcfs::from_cmdline(cmdline), proc.pid());

    constexpr uint64_t i = 0x1234'1234'1234'1234;
    constexpr double fp = M_PI;
    const mem_t mem_default(i, fp);
    mem_t mem(0, 0);
    BOOST_REQUIRE_NE(&mem_default, &mem);
    BOOST_REQUIRE_NE(mem_default._u64, mem._u64);
    BOOST_REQUIRE_NE(mem_default._f64, mem._f64);
    BOOST_REQUIRE_NE((void*)mem_default._cs.data(), (void*)mem._cs.data());

    BOOST_CHECK_MESSAGE(proc.read(reinterpret_cast<uintptr_t>(&mem_default._u8), &mem._u8, sizeof(mem._u8)) == sizeof(mem._u8), "read (1 byte integer)");
    BOOST_CHECK_NE(&mem_default._u8, &mem._u8);
    BOOST_CHECK_EQUAL(mem_default._u8, mem._u8);

    BOOST_CHECK_MESSAGE(proc.read(reinterpret_cast<uintptr_t>(&mem_default._u64), &mem._u64, sizeof(mem._u64)) == sizeof(mem._u64), "read (8 byte integer)");
    BOOST_CHECK_EQUAL(mem_default._u64, mem._u64);

    BOOST_CHECK_MESSAGE(proc.read(reinterpret_cast<uintptr_t>(&mem_default._f64), &mem._f64, sizeof(mem._f64)) == sizeof(mem._f64), "read (8 byte float)");
    BOOST_CHECK_EQUAL(mem_default._f64, mem._f64);

    // BOOST_CHECK_MESSAGE(proc.read(reinterpret_cast<uintptr_t>(&mem_default), &mem, sizeof(mem)) == sizeof(mem), "read string");
}

BOOST_AUTO_TEST_CASE(ProcessCRIU_functional)
{
    std::filesystem::path some("/tmp/to/somedir");
    {
        // todo[high] ProcessCRIU test: fork self test
        PWNER::PROCESS::ProcessCRIU proc(some);
        proc.set_leave_running(true)
            .set_shell_job(false)
            .set_pid(getpid());
        proc.dump();
    }
    {
        PWNER::PROCESS::ProcessCRIU proc(some);
        proc.set_shell_job(false);
        if (pid_t p = proc.restore_child(); p > 0) {
            std::cout<<"killing self"<<std::endl;
            kill(p, 9);
        }
    }
}

BOOST_AUTO_TEST_CASE(ProcessHeap_functional)
{
    PWNER::PROCESS::ProcessProcfs proc_procfs(getpid());
    BOOST_REQUIRE(proc_procfs && proc_procfs.running());
    PWNER::PROCESS::ProcessHeap proc(proc_procfs);
    BOOST_CHECK(proc);

    // todo[low] ProcessHeap test: fork self test?
    BOOST_CHECK_MESSAGE([&]() -> bool {
        // proc_heap.
        return false;
    }(), "Returning false");

    BOOST_CHECK_MESSAGE([]() -> bool {
        return true;
    }(), "Returning true");
}

BOOST_AUTO_TEST_CASE(ProcessFile_functional)
{
    PWNER::PROCESS::ProcessProcfs proc_procfs(getpid());
    BOOST_REQUIRE(proc_procfs && proc_procfs.running());

    PWNER::PROCESS::ProcessFile proc(proc_procfs, std::filesystem::current_path()/(std::to_string(proc_procfs.pid())+".pwn"));
    BOOST_CHECK(proc);

    BOOST_CHECK_MESSAGE([&]() -> bool {
        // proc_heap.
        return false;
    }(), "Returning false");

    BOOST_CHECK_MESSAGE([]() -> bool {
        return true;
    }(), "Returning true");
}

// BOOST_AUTO_TEST_CASE(Process_functional)
// {
//     //TODO[high]: PROCESS: polymorphism and other things
//     std::shared_ptr<PWNER::PROCESS::Process> proc = std::make_shared<PWNER::PROCESS::ProcessProcfs>(getpid());
//
// }

BOOST_AUTO_TEST_SUITE_END()
