#include "TimesliceBuilder.hpp"

#include "MicrosliceSource.hpp" // needs microslice branch

namespace fles {

TimesliceBuilder::TimesliceBuilder(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
    _last = end(_timeslices); // invalidate
}

std::unique_ptr<StorableTimeslice> TimesliceBuilder::get()
{
    auto ts = static_cast<StorableTimeslice *>(nullptr);

    // Find the position of the next timeslice we want to return, which
    // is the the one with the lowest index higher than what we returned
    // the last time (may have been added in the meantime).
    // If we come here for the first time, `_last` has not yet been set
    // and we pick the first timeslice manually.
    auto it = (_last != end(_timeslices)) ? next(_last) : begin(_timeslices);

    // If there is such a timeslice, we remember its position and move it
    // out of the map (where it is no longer needed) onto the heap, from
    // where it will be picked up by a unique_ptr.
    if (it != end(_timeslices)) {
        _last = it;
        ts = new StorableTimeslice {std::move(it->second)};
    }

    return std::unique_ptr<StorableTimeslice> {ts};
}

void TimesliceBuilder::add_microslices(MicrosliceSource& mc_source)
{
    auto num_microslices = mc_source.size();
    for (auto ts_index = mc_source.start_index / _ts_len;
         ts_index * _ts_len < mc_source.start_index + num_microslices;
         ts_index++) {
        auto& ts = _timeslices.emplace(ts_index, _ts_len).first->second;
        auto comp_index = ts.append_component(_ts_len, ts_index);
        auto mc_index = ts_index * _ts_len;
        for (size_t i = 0; i < _ts_len; i++) {
            auto mc = mc_source.get(mc_index + i);
            ts.append_microslice(comp_index, i, mc);
        }
    }
}

} // namespace
