#pragma once

// fles_ipc
#include "StorableTimeslice.hpp"
// system
#include <map>
#include <memory>

namespace fles {

class MicrosliceSource;

class TimesliceBuilder {
public:
    TimesliceBuilder(size_t ts_len, uint64_t start_index=0);
    void add_microslices(MicrosliceSource& mc_source);
    std::unique_ptr<StorableTimeslice> get();

private:
    std::map<uint64_t, StorableTimeslice> _timeslices;
    decltype(_timeslices)::iterator _it, _last;
    size_t _ts_len;
    uint64_t _start_idx;
};

} // namespace
