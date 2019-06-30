#pragma once

#include <pwner/common.hh>
#include <pwner/external.hh>
#include <pwner/process/IO/IO.hh>
#include <pwner/wrapper/linux_fd.hh>
#include <criu/criu.h>


NAMESPACE_BEGIN(pwner)
NAMESPACE_BEGIN(PROCESS)

class IOCRIU : virtual public IO {
public:
    enum class service_comm {
        SK = CRIU_COMM_SK,
        FD = CRIU_COMM_FD,
        BIN = CRIU_COMM_FD,
    };

    enum class cg_mode {
        IGNORE = CRIU_CG_MODE_IGNORE,
        NONE = CRIU_CG_MODE_NONE,
        PROPS = CRIU_CG_MODE_PROPS,
        SOFT = CRIU_CG_MODE_SOFT,
        FULL = CRIU_CG_MODE_FULL,
        STRICT = CRIU_CG_MODE_STRICT,
        DEFAULT = CRIU_CG_MODE_DEFAULT,
    };

    explicit IOCRIU(pid_t pid, const std::filesystem::path& path) noexcept
    : IOCRIU(path) {
        set_pid(pid);
    }

    explicit IOCRIU(const std::filesystem::path& path) noexcept
    : dir_fd(path.c_str(), O_DIRECTORY), opts(nullptr) {
        if (int rc = criu_local_init_opts(&opts); rc < 0) { //! return non-negative on success
            std::cerr<<"IOCRIU::IOCRIU(): criu_local_init_opts, rc: "<<rc<<std::endl;
            opts = nullptr;
            return;
        }
        criu_local_set_images_dir_fd(opts, dir_fd);
    }

    ~IOCRIU() override {
        criu_local_free_opts(opts); // TODO[critical]: add dependency CRIU == 3.12
    };

    bool reset() {
        if (opts)
            criu_local_free_opts(opts);
        return criu_local_init_opts(&opts) >= 0;
    }

    IOCRIU& set_service_address(const std::filesystem::path& path) { criu_local_set_service_address(opts, const_cast<char *>(path.c_str())); return *this;}
    IOCRIU& set_service_fd(int fd) { criu_local_set_service_fd(opts, fd); return *this; }

    IOCRIU& set_pid(pid_t pid) {criu_local_set_pid(opts, static_cast<int>(pid)); return *this;}
    IOCRIU& set_images_dir_fd(int fd) { criu_local_set_images_dir_fd(opts, fd); return *this;}
    IOCRIU& set_images_dir_fd(const std::filesystem::path& path) { dir_fd = linux_fd(path.c_str(), O_DIRECTORY); criu_local_set_images_dir_fd(opts, dir_fd); return *this;}
    IOCRIU& set_parent_images(const std::filesystem::path& path) {criu_local_set_parent_images(opts, const_cast<char *>(path.c_str())); return *this;}
    // IOCRIU& set_service_binary(const std::filesystem::path& path) {criu_local_set_service_binary(opts, path.c_str()); return *this;}
    IOCRIU& set_service_binary(const char *path) {criu_local_set_service_binary(opts, path); return *this;}
    IOCRIU& set_work_dir_fd(int fd) {criu_local_set_work_dir_fd(opts, fd); return *this;}
    IOCRIU& set_leave_running(bool leave_running) {criu_local_set_leave_running(opts, leave_running); return *this;}
    IOCRIU& set_ext_unix_sk(bool ext_unix_sk) {criu_local_set_ext_unix_sk(opts, ext_unix_sk); return *this;}
    // int add_unix_sk(unsigned int inode) {criu_local_add_unix_sk(opts, inode); return *this;}
    IOCRIU& set_tcp_established(bool tcp_established) {criu_local_set_tcp_established(opts, tcp_established); return *this;}
    IOCRIU& set_tcp_skip_in_flight(bool tcp_skip_in_flight) {criu_local_set_tcp_skip_in_flight(opts, tcp_skip_in_flight); return *this;}
    IOCRIU& set_weak_sysctls(bool val) {criu_local_set_weak_sysctls(opts, val); return *this;}
    IOCRIU& set_evasive_devices(bool evasive_devices) {criu_local_set_evasive_devices(opts, evasive_devices); return *this;}
    IOCRIU& set_shell_job(bool shell_job) {criu_local_set_shell_job(opts, shell_job); return *this;}
    IOCRIU& set_file_locks(bool file_locks) {criu_local_set_file_locks(opts, file_locks); return *this;}
    IOCRIU& set_track_mem(bool track_mem) {criu_local_set_track_mem(opts, track_mem); return *this;}
    IOCRIU& set_auto_dedup(bool auto_dedup) {criu_local_set_auto_dedup(opts, auto_dedup); return *this;}
    IOCRIU& set_force_irmap(bool force_irmap) {criu_local_set_force_irmap(opts, force_irmap); return *this;}
    IOCRIU& set_link_remap(bool link_remap) {criu_local_set_link_remap(opts, link_remap); return *this;}
    IOCRIU& set_log_level(int log_level) {criu_local_set_log_level(opts, log_level); return *this;}
    // IOCRIU& set_log_file(const std::filesystem::path& filename) {criu_local_set_log_file(opts, const_cast<char *>(filename.filename().c_str())); return *this;}
    IOCRIU& set_log_file(const char *filename) {criu_local_set_log_file(opts, const_cast<char *>(filename)); return *this;}
    IOCRIU& set_cpu_cap(unsigned int cap) {criu_local_set_cpu_cap(opts, cap); return *this;}
    IOCRIU& set_root(const std::filesystem::path& path) {criu_local_set_root(opts, const_cast<char*>(path.c_str())); return *this;}
    IOCRIU& set_manage_cgroups(bool manage) {criu_local_set_manage_cgroups(opts, manage); return *this;}
    IOCRIU& set_manage_cgroups_mode(const cg_mode& mode) {criu_local_set_manage_cgroups_mode(opts, static_cast<enum criu_cg_mode>(mode)); return *this;}
    IOCRIU& set_freeze_cgroup(const std::string& name) {criu_local_set_freeze_cgroup(opts, const_cast<char *>(name.c_str())); return *this;}
    IOCRIU& set_timeout(unsigned int timeout) {criu_local_set_timeout(opts, timeout); return *this;}
    IOCRIU& set_auto_ext_mnt(bool val) {criu_local_set_auto_ext_mnt(opts, val); return *this;}
    IOCRIU& set_ext_sharing(bool val) {criu_local_set_ext_sharing(opts, val); return *this;}
    IOCRIU& set_ext_masters(bool val) {criu_local_set_ext_masters(opts, val); return *this;}
    // int set_exec_cmd(int argc, char *argv[]) {criu_local_set_exec_cmd(opts, XXX, YYY); return *this;}
    // int add_ext_mount(char *key, char *val) {criu_local_add_ext_mount(opts, XXX, YYY); return *this;}
    // int add_veth_pair(char *in, char *out) {criu_local_add_veth_pair(opts, XXX, YYY); return *this;}
    // int add_cg_root(char *ctrl, char *path) {criu_local_add_cg_root(opts, XXX, YYY); return *this;}
    // int add_enable_fs(char *fs) {criu_local_add_enable_fs(opts, XXX, YYY); return *this;}
    // int add_skip_mnt(char *mnt) {criu_local_add_skip_mnt(opts, XXX, YYY); return *this;}
    IOCRIU& set_ghost_limit(unsigned int limit) {criu_local_set_ghost_limit(opts, limit); return *this;}
    // int add_irmap_path(char *path) {criu_local_add_irmap_path(opts, XXX, YYY); return *this;}
    // int add_cg_props(char *stream) {criu_local_add_cg_props(opts, XXX, YYY); return *this;}
    // int add_cg_props_file(char *path) {criu_local_add_cg_props_file(opts, XXX, YYY); return *this;}
    // int add_cg_dump_controller(char *name) {criu_local_add_cg_dump_controller(opts, XXX, YYY); return *this;}
    // int add_inherit_fd(int fd, char *key) {criu_local_add_inherit_fd(opts, XXX, YYY); return *this;}
    // int add_external(char *key) {criu_local_add_external(opts, XXX, YYY); return *this;}

    IOCRIU& set_notify_cb(int (*cb)(char *action, criu_notify_arg_t na)) {criu_local_set_notify_cb(opts, cb); return *this;}

    int check() { return criu_local_check(opts); } //! return 0 on success and sets errno
    int dump() { return criu_local_dump(opts); } //! return non-negative on success and sets errno
    pid_t restore() { return criu_local_restore(opts); } //! return non-negative PID on success and sets errno
    pid_t restore_child() { return criu_local_restore_child(opts); } //! return non-negative pid on success and sets errno
    // int dump_iters(int (*more)(criu_predump_info pi)) { return criu_local_dump_iters(opts, more); }


    explicit operator bool() const override {
        return opts != nullptr;
    }

    // TODO[critical]: to impl
    size_t read(uintptr_t address, void *out, size_t size) const override {
        return npos;
    }

    size_t write(uintptr_t address, void *in, size_t size) const override {
        return npos;
    }

public:
    linux_fd dir_fd;
    criu_opts *opts;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(pwner)
