#pragma once

#include <pwner/process/IO/IO.hh>


NAMESPACE_BEGIN(pwner)
NAMESPACE_BEGIN(PROCESS)

class IOMappedHeap : public virtual IOMapped {
public:
    explicit IOMappedHeap(const IO &io) {
        regions = io.regions;
        segments = io.segments;

        regions_mapped.clear();
        for (const Region& region : regions) {
            // std::shared_ptr<char[]> map = std::make_shared<char[]>(region.size);
            std::shared_ptr<char[]> map = std::shared_ptr<char[]>(new char[region.size], [](char *x) {delete[] x;});
            regions_mapped.push_back(map);

            // FIXME[med]: bug: can not copy some regions.
            size_t copied = io.read(region.address, map.get(), region.size);
            if (copied == npos) {
                memset(map.get(), 0, region.size);
                std::clog<<"Cannot read region: "<<region<<std::endl;
            }
        }
    };

    explicit operator bool() const override {
        return !regions_mapped.empty();
    };
};

NAMESPACE_END(PROCESS)
NAMESPACE_END(pwner)
