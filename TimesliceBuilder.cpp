#include "TimesliceBuilder.hpp"

#include "MicrosliceSource.hpp" // needs microslice branch

namespace fles {

TimesliceBuilder::TimesliceBuilder(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
    // invalidate both iterators
    _it = end(_timeslices);
    _last = end(_timeslices);
}

std::unique_ptr<StorableTimeslice> TimesliceBuilder::get()
{
    auto ts_p = static_cast<StorableTimeslice *>(nullptr);

    // The first time we get here, we have to initialize `_it` manually to
    // the beginning. From then on, `_last` will always point to what we
    // returned previously and we position `_it` next to that, which could
    // be off the end. If we had reached the end before, but additional
    // entries have been added in the meantime, this ensures that we
    // continue at the correct position.
    if (_last == end(_timeslices)) {
        _it = begin(_timeslices);
    } else {
        _it = next(_last);
    }

    // If there is an output value, we remember the position and move the
    // timeslice out of the map onto the heap, from where it will be
    // managed by a unique_ptr.
    if (_it != end(_timeslices)) {
        _last = _it;
        ts_p = new StorableTimeslice {std::move(_it->second)};
    }

    return std::unique_ptr<StorableTimeslice> {ts_p};
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
