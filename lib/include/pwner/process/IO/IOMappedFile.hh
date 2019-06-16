#pragma once

#include <pwner/process/IO/IO.hh>
//
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>


NAMESPACE_BEGIN(PWNER)
NAMESPACE_BEGIN(PROCESS)

class IOMappedFile : public IOMapped {
public:
    explicit IOMappedFile(const PWNER::PROCESS::IO& handler, const std::filesystem::path &path) {
        save(handler, path);
    }

    explicit IOMappedFile(const std::filesystem::path &path) {
        load(path);
    }

    void save(const PWNER::PROCESS::IO& handler, const std::filesystem::path &path) {
        regions = handler.regions;

        std::ofstream stream(path, std::ios_base::out | std::ios_base::binary);
        boost::archive::binary_oarchive archive(stream, boost::archive::no_header);
        archive << *this;
        stream.flush();

        params.path = path;
        params.flags = boost::iostreams::mapped_file::mapmode::readwrite;
        mf.open(params);
        if (!mf.is_open())
            throw std::invalid_argument("can not open '" + path.native() + "'");

        size_t bytes_to_save = 0;
        for (const Region& region : handler.regions)
            bytes_to_save += sizeof(region.size) + region.size;
        mf.resize(mf.size() + bytes_to_save);

        char* snapshot = mf.data();
        assert(stream.is_open());
        assert(stream.tellp() != -1);
        snapshot += stream.tellp();

        regions_mapped.clear();
        for(const Region& region : handler.regions) {
            regions_mapped.emplace_back(snapshot, [](char *x) {});
            ssize_t copied = handler.read(region.address, snapshot, region.size);
            //snapshot += region.size + sizeof(mem64_t::bytes) - 1;
            snapshot += region.size;
        }
    }

    void load(const std::filesystem::path &path) {
        std::ifstream stream(path, std::ios_base::in | std::ios_base::binary);
        std::string m(magic);
        stream.read(&m[0], m.size());

        if (!stream || m != magic) {
            throw std::runtime_error("magic bytes not matched");
        }
        boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
        archive >> *this;

        params.path = path;
        params.flags = boost::iostreams::mapped_file::mapmode::readwrite;
        mf.open(params);
        if (!mf.is_open())
            throw std::invalid_argument("can not open '" + path.native() + "'");

        char* snapshot = mf.data();
        assert(stream.is_open());
        assert(stream.tellg() != -1);
        snapshot += stream.tellg();

        regions_mapped.clear();
        for (const Region& region : regions) {
            regions_mapped.emplace_back(snapshot, [](char *x) {});
            //snapshot += region.size + sizeof(mem64_t::bytes) - 1;
            snapshot += region.size;
        }
    }

    explicit operator bool() const override {
        // return bool(mf);
        return true;
    }

private:
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        // ar & regions;
        ar & magic;
    }

protected:
    std::string magic = "PWNER";
    boost::iostreams::mapped_file mf;
    boost::iostreams::mapped_file_params params;
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(PWNER)
