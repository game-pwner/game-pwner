

#include <regex>
#include <pwner/common.hh>
#include <pwner/external.hh>
#include <pwner/process/Regions.hh>
#include <pwner/process/IO.hh>
#include <cinttypes>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class IOProcfs : virtual public IO {
public:
    explicit IOProcfs(pid_t pid)
    : m_pid(pid) {
        update_regions();
    }

    explicit IOProcfs(const std::regex &regex_pattern_cmdline)
    : m_pid(from_cmdline(regex_pattern_cmdline)) {
        update_regions();
    }

    virtual ~IOProcfs() = default;

    static bool running(pid_t pid) { return sfs::exists(sfs::path("/proc") / std::to_string(pid)); }

    static std::filesystem::path cwd(pid_t pid) { return sfs::read_symlink(sfs::path("/proc") / std::to_string(pid) / "cwd"); }

    static std::filesystem::path exe(pid_t pid) { return sfs::read_symlink(sfs::path("/proc") / std::to_string(pid) / "exe"); }

    static std::string cmdline(pid_t pid) {
        std::ifstream t(sfs::path("/proc") / std::to_string(pid) / "cmdline");
        std::ostringstream ss;
        ss<<t.rdbuf();
        return ss.str();
    }

    static pid_t from_cmdline(const std::regex &pattern_cmdline) {
        for(auto& p: sfs::directory_iterator("/proc")) {
            if (p.path().filename().string().find_first_not_of("0123456789") != std::string::npos)
                /* if filename is not numeric */
                continue;
            if (!sfs::is_directory(p))
                continue;
            if (!sfs::exists(p / "maps"))
                continue;
            if (!sfs::exists(p / "exe"))
                continue;
            if (!sfs::exists(p / "cmdline"))
                continue;
            std::istringstream ss(p.path().filename().string());
            pid_t pid;
            ss >> pid;
            std::string m_cmdline = cmdline(pid);

            std::smatch match;
            if (std::regex_search(m_cmdline, match, pattern_cmdline)) {
                return pid;
            }
        }
        return 0;
    }

    bool running() const { return running(m_pid); };
    // bool running() { return sfs::exists(sfs::path("/proc") / std::to_string(m_pid)); };

    std::filesystem::path cwd() const { return cwd(m_pid); };

    std::filesystem::path exe() const { return exe(m_pid); };

    std::string cmdline() const { return cmdline(m_pid); };

    pid_t pid() const { return m_pid; };

    void update_regions() override {
        regions.clear();
        regions.emplace_back(Region{});
        std::ifstream maps(sfs::path("/proc") / std::to_string(pid()) / "maps");

        std::string line;
        while (getline(maps, line)) {
            // todo[med]: rewrite as it done in tools/ad/ad.h
            // 00601000-00602000 rw-p 00001000 08:11 3427227      /bin/FAKEMEM
            std::vector<std::string> cols = tokenize(line, " ", 6);
            if (cols.size() < 5)
                continue;
            Region r;
            std::vector<std::string> mem = tokenize(cols[0], "-");
            r.address = strtoumax(mem[0].c_str(), nullptr, 16);
            r.size = strtoumax(mem[1].c_str(), nullptr, 16) - r.address;
            r.mode |= (cols[1][0] == 'r') ? region_mode_t::readable : region_mode_t::none;
            r.mode |= (cols[1][1] == 'w') ? region_mode_t::writable : region_mode_t::none;
            r.mode |= (cols[1][2] == 'x') ? region_mode_t::executable : region_mode_t::none;
            r.mode |= (cols[1][3] == 's') ? region_mode_t::shared : region_mode_t::none;
            r.offset = strtoumax(cols[2].c_str(), nullptr, 16);
            std::vector<std::string> dev = tokenize(cols[3], ":");
            r.st_device_major = strtoumax(dev[0].c_str(), nullptr, 16);
            r.st_device_minor = strtoumax(dev[1].c_str(), nullptr, 16);
            r.inode = strtoumax(cols[4].c_str(), nullptr, 10);
            if (cols.size() > 5)
                r.file = sfs::path(cols[5]);
            regions.push_back(r);
        }

        for (size_t i = 0; i < regions.size(); i++) {
            Region& r = regions[i];
            r.id = i;
            if (r.offset != 0 || (r.mode & region_mode_t::writable) == 1 || !sfs::exists(r.file)) {
                // std::clog << "Procfs::update_regions(): skipping region: " << r << std::endl;
                continue;
            }
            r.parent = 0;
            // std::clog << "Procfs::update_regions(): parsing region: " << r << std::endl;
            for (size_t ii = i + 1; ii < regions.size(); ii++) {
                Region& rr = regions[ii];
                if (rr.file != r.file) {
                    break;
                }
                // std::clog << "Procfs::update_regions(): parsing region: parent: " << rr << std::endl;
                rr.parent = i;
            }
        }

        Regions::update_regions();
    }

    explicit operator bool() const override {
        return m_pid != 0;
    }

    size_t read(uintptr_t address, void *out, size_t size) const override {
        struct iovec local[1];
        struct iovec remote[1];
        local[0].iov_base = out;
        local[0].iov_len = size;
        remote[0].iov_base = reinterpret_cast<void *>(address);
        remote[0].iov_len = size;
        return static_cast<size_t>(process_vm_readv(m_pid, local, 1, remote, 1, 0));
    }

    size_t write(uintptr_t address, void *in, size_t size) const override {
        struct iovec local[1];
        struct iovec remote[1];
        local[0].iov_base = in;
        local[0].iov_len = size;
        remote[0].iov_base = reinterpret_cast<void *>(address);
        remote[0].iov_len = size;
        return static_cast<size_t>(process_vm_writev(m_pid, local, 1, remote, 1, 0));
    }

protected:
    pid_t m_pid;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
