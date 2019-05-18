//
// Created by root on 12.04.19.
//

#pragma once

#include <r_io.h>
#include <r_bin.h>
#include <pwner/common.hh>
#include <pwner/scanner/Value.hh>


NAMESPACE_BEGIN(R2)

namespace sfs = std::filesystem;

class Bin {
public:
    Bin() {
        bin = std::shared_ptr<RBin>(r_bin_new(), [](RBin *x) { r_bin_free(x); });
        if (!bin) {
            std::clog << "Bin::Bin(): !bin" << std::endl;
            return;
        }
        io = std::shared_ptr<RIO>(r_io_new(), [](RIO *x) { r_io_free(x); });
        if (!bin) {
            std::clog << "Bin::Bin(): !io" << std::endl;
            return;
        }
        r_io_bind(io.get(), &bin->iob);
    }

    explicit Bin(const sfs::path &file)
            : Bin() {
        if (!open(file)) {
            std::clog << "Bin::Bin(file): !open: file: " << file << std::endl;
            return;
        }
    }

    bool open(const sfs::path &file) {
        /// open(file) will be faster later:
        // pancake 🎗, [20.04.19 18:40]: Rbin loads the file in memory
        // pancake 🎗, [20.04.19 18:40]: Its totally unrelated to the io
        // pancake 🎗, [24.04.19 14:20]: the rbin apis will be rewritten completely
        if (!sfs::exists(file)) {
            std::clog << "Bin::open(): !exists: file: " << file << std::endl;
            return false;
        }

        // r_bin_options_init (&opt, -1, UT64_MAX, UT64_MAX, false/*true*/);
        // opt.xtr_idx = xtr_idx;
        if (!r_bin_open(bin.get(), file.c_str(), &opt)) {
            std::clog << "Bin::open(): !r_bin_open: file: " << file << std::endl;
            return false;
        }

        return true;
    }

    bool is_opened() const {
        RIODesc *desc = r_io_desc_get(io.get(), opt.fd);
        return desc != nullptr;
    }

    explicit operator bool() const {
        return bin && io.operator bool() && is_opened();
    }

    std::vector<RBinString *> get_strings() {
        std::vector<RBinString *> ret;

        RList *list = r_bin_get_symbols(bin.get());
        if (list)
            for (RListIter *iter = list->head; iter; iter = iter->n)
                ret.emplace_back(static_cast<RBinString *>(iter->data));

        return ret;
    }

    std::vector<RBinSymbol *> get_symbols() {
        std::vector<RBinSymbol *> ret;

        RList *list = r_bin_get_symbols(bin.get());
        if (list)
            for (RListIter *iter = list->head; iter; iter = iter->n)
                ret.emplace_back(static_cast<RBinSymbol *>(iter->data));

        return ret;
    }

    std::vector<RBinSection *> get_sections() {
        std::vector<RBinSection *> ret;

        RList *list = r_bin_get_sections(bin.get());
        if (list)
            for (RListIter *iter = list->head; iter; iter = iter->n)
                ret.emplace_back(static_cast<RBinSection *>(iter->data));

        return ret;
    }

    std::vector<RBinSection *> get_segments() {
        std::vector<RBinSection *> ret;

        RList *list = r_bin_get_sections(bin.get());
        if (list)
            for (RListIter *iter = list->head; iter; iter = iter->n)
                ret.emplace_back(static_cast<RBinSection *>(iter->data));

        return ret;
    }

public:
    RBinOptions opt{};
    std::shared_ptr<RBin> bin;
    std::shared_ptr<RIO> io;
};

NAMESPACE_END(R2)
