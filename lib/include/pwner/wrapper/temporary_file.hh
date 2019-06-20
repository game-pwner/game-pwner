#pragma once

#include <filesystem>


class temporary_file {
public:
    explicit temporary_file() = default;

    explicit temporary_file(size_t __n) {
        create(__n);
    }

    ~temporary_file() {
        destroy();
    }

    void create(size_t __n = 0) {
        destroy();
        path = tmpnam(nullptr); // todo[high]: mkstemp
        std::ofstream of(path);
        of.close();
        std::filesystem::resize_file(path, __n);
    }

    void destroy() {
        if (std::filesystem::exists(path))
            std::filesystem::remove(path);
        path = "";
    }

    explicit operator bool() const {
        return std::filesystem::exists(path);
    }

    std::filesystem::path path;
};
