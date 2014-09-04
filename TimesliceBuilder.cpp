#include "TimesliceBuilder.hpp"

#include "MicrosliceSource.hpp" // needs microslice branch

namespace fles {

TimesliceBuilder::TimesliceBuilder(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
    _last = end(_timeslices); // invalidate
}

void TimesliceBuilder::add_microslices(MicrosliceSource& mc_source)
{
    // calculate the range of timeslices we need to access
    auto ts_first = mc_source.start_index / _ts_len;
    auto ts_last = (mc_source.start_index + mc_source.size()) / _ts_len;

    for (auto ts_idx = ts_first; ts_idx < ts_last; ts_idx++) {
        // get the current timeslice or create it if it doesn't exist
        auto& ts = _timeslices.emplace(ts_idx, _ts_len).first->second;

        // add all microslices belonging to this timeslice to a new component
        auto comp_idx = ts.append_component(_ts_len, ts_idx);
        auto mc_base_idx = ts_idx * _ts_len;
        for (size_t i = 0; i < _ts_len; i++) {
            auto mc = mc_source.get(mc_base_idx + i);
            ts.append_microslice(comp_idx, i, mc);
        }
    }
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

} // namespace
