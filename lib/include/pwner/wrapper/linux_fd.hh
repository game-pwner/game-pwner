#pragma once

#include <unistd.h>  // close


class linux_fd {
public:
    /* for retards: std::unique_ptr<int, std::function<void(const int*)>> p(new int(open(path, flags)), [](const int *x) { close(*x); }); */
    explicit linux_fd(const char *path, int flags)
            : linux_fd(open(path, flags)) {}

    explicit linux_fd(int fd)
            : fd(fd) {}

    virtual ~linux_fd() noexcept { if (fd >= 3) close(fd); }  // {0: stdin, 1: stdout, 2: stderr, ...}

    operator int() const { return fd; } // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

public:
    int fd;
};
