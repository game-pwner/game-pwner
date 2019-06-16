#pragma once

#include <regex>
#include <pwner/common.hh>
#include <pwner/external.hh>
#include <pwner/process/Regions.hh>
#include <pwner/process/IO/IO.hh>
#include <cinttypes>
#include <sys/uio.h> // process_vm_readv


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class IOProcfs : virtual public IO {
public:
    IOProcfs()
    : m_pid(0) {}

    explicit IOProcfs(pid_t pid)
    : m_pid(pid) {
        update_regions();
    }

    explicit IOProcfs(const std::string &regex_pattern_cmdline)
    : m_pid(from_cmdline(regex_pattern_cmdline)) {
        update_regions();
    }

    static bool running(pid_t pid) { return std::filesystem::exists(std::filesystem::path("/proc") / std::to_string(pid)); }

    static std::filesystem::path cwd(pid_t pid) { return std::filesystem::read_symlink(std::filesystem::path("/proc") / std::to_string(pid) / "cwd"); }

    static std::filesystem::path exe(pid_t pid) { return std::filesystem::read_symlink(std::filesystem::path("/proc") / std::to_string(pid) / "exe"); }

    static std::string cmdline(pid_t pid) {
        std::ifstream t(std::filesystem::path("/proc") / std::to_string(pid) / "cmdline");
        std::ostringstream ss;
        ss<<t.rdbuf();
        return ss.str();
    }

    static pid_t from_cmdline(const std::string &pattern_cmdline) {
        const std::regex pattern(pattern_cmdline);
        for(auto& p: std::filesystem::directory_iterator("/proc")) {
            if (p.path().filename().string().find_first_not_of("0123456789") != std::string::npos)
                /* if filename is not numeric */
                continue;
            if (!std::filesystem::is_directory(p))
                continue;
            if (!std::filesystem::exists(p / "maps"))
                continue;
            if (!std::filesystem::exists(p / "exe"))
                continue;
            if (!std::filesystem::exists(p / "cmdline"))
                continue;
            std::istringstream ss(p.path().filename().string());
            pid_t pid;
            ss >> pid;
            std::string m_cmdline = cmdline(pid);

            if (std::regex_search(m_cmdline, pattern)) {
                return pid;
            }
        }
        return 0;
    }

    bool running() const { return running(m_pid); };

    std::filesystem::path cwd() const { return cwd(m_pid); };

    std::filesystem::path exe() const { return exe(m_pid); };

    std::string cmdline() const { return cmdline(m_pid); };

    pid_t pid() const { return m_pid; };

    void update_regions() override {
        regions.clear();
        regions.emplace_back();
        std::ifstream maps(std::filesystem::path("/proc") / std::to_string(pid()) / "maps");
        // Timer __t("update_regions");

        std::string line;
        while (getline(maps, line)) {
            // 00601000-00602000 rw-p 00001000 08:11 3427227      /bin/FAKEMEM
            Region r;
            const char *linec = const_cast<char *>(line.c_str());
            r.address = strtoumax(linec, const_cast<char **>(&linec), 16);
            linec++;
            r.size = strtoumax(linec, const_cast<char **>(&linec), 16) - r.address;
            linec++;
            r.mode |= (linec[0] == 'r') ? region_mode_t::readable : region_mode_t::none;
            r.mode |= (linec[1] == 'w') ? region_mode_t::writable : region_mode_t::none;
            r.mode |= (linec[2] == 'x') ? region_mode_t::executable : region_mode_t::none;
            r.mode |= (linec[3] == 's') ? region_mode_t::shared : region_mode_t::none;
            linec += 5;
            r.offset = strtoumax(linec, const_cast<char **>(&linec), 16);
            linec++;
            r.st_device_major = static_cast<uint8_t>(strtoumax(linec, const_cast<char **>(&linec), 16));
            linec++;
            r.st_device_minor = static_cast<uint8_t>(strtoumax(linec, const_cast<char **>(&linec), 16));
            linec++;
            r.inode = strtoumax(linec, const_cast<char **>(&linec), 10);
            while(isspace(*linec))
                linec++;
            r.file = std::filesystem::path(linec);
            regions.push_back(r);
        }

        // #include <inttypes.h>  // SCNx64, SCNx8, SCNu64
        // std::string line;
        // while (getline(maps, line)) {
        //     // 00601000-00602000 rw-p 00001000 08:11 3427227      /bin/FAKEMEM
        //     const char *f = "%" SCNx64 "-%" SCNx64 " %4s %" SCNx64 " %" SCNx8 ":%" SCNx8 " %" SCNu64 " %[^\n]s";
        //
        //     uint64_t h_addr_begin;
        //     uint64_t h_addr_end;
        //     char mode[5];
        //     uint64_t h_off;
        //     uint8_t h_dev1;
        //     uint8_t h_dev2;
        //     uint64_t d_inode;
        //     char filepath[1u<<10u];
        //
        //     sscanf(line.c_str(), f, &h_addr_begin, &h_addr_end, &mode, &h_off, &h_dev1, &h_dev2, &d_inode, &filepath);
        //
        //     Region r;
        //     r.address = h_addr_begin;
        //     r.size = h_addr_end - h_addr_begin;
        //     r.mode |= (mode[0] == 'r') ? region_mode_t::readable : region_mode_t::none;
        //     r.mode |= (mode[1] == 'w') ? region_mode_t::writable : region_mode_t::none;
        //     r.mode |= (mode[2] == 'x') ? region_mode_t::executable : region_mode_t::none;
        //     r.mode |= (mode[3] == 's') ? region_mode_t::shared : region_mode_t::none;
        //     r.offset = h_off;
        //     r.st_device_major = h_dev1;
        //     r.st_device_minor = h_dev2;
        //     r.inode = d_inode;
        //     r.file = std::filesystem::path(filepath);
        //     regions.push_back(r);
        // }

        // std::string line;
        // while (getline(maps, line)) {
        //     // 00601000-00602000 rw-p 00001000 08:11 3427227      /bin/FAKEMEM
        //     std::istringstream ss(line);
        //     std::uintptr_t hex_addr_beg;
        //     std::uintptr_t hex_addr_end;
        //     std::string mode;
        //     std::uintptr_t hex_offset;
        //     std::uintptr_t hex_dev_major;
        //     std::uintptr_t hex_dev_minor;
        //     std::uintptr_t inode;
        //     char skip_c;
        //     std::string path;
        //     if (ss >> std::hex >> hex_addr_beg >> skip_c >> hex_addr_end
        //            >> mode >> hex_offset >> hex_dev_major >> skip_c >> hex_dev_minor >> std::dec >> inode >> std::ws) {
        //         getline(ss, path);
        //         Region r;
        //         r.address = hex_addr_beg;
        //         r.size = hex_addr_end - hex_addr_beg;
        //         r.mode |= (mode[0] == 'r') ? region_mode_t::readable : region_mode_t::none;
        //         r.mode |= (mode[1] == 'w') ? region_mode_t::writable : region_mode_t::none;
        //         r.mode |= (mode[2] == 'x') ? region_mode_t::executable : region_mode_t::none;
        //         r.mode |= (mode[3] == 's') ? region_mode_t::shared : region_mode_t::none;
        //         r.offset = hex_offset;
        //         r.st_device_major = static_cast<uint8_t>(hex_dev_major);
        //         r.st_device_minor = static_cast<uint8_t>(hex_dev_minor);
        //         r.inode = inode;
        //         r.file = path;
        //         regions.push_back(r);
        //         std::clog << "r: " << r << std::endl;
        //     }
        // }

        for (size_t i = 0; i < regions.size(); i++) {
            Region& r = regions[i];
            r.id = i;
            if (r.offset != 0 || (r.mode & region_mode_t::writable) == 1 || !std::filesystem::exists(r.file)) {
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
        return m_pid > 0;
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
