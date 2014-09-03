#include "TimesliceBuilder.hpp"

#include "MicrosliceSource.hpp" // needs microslice branch

namespace fles {

TimesliceBuilder::TimesliceBuilder(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
    _it = end(_timeslices);
    _last = end(_timeslices);
}

std::unique_ptr<StorableTimeslice> TimesliceBuilder::get()
{
    if (_last != end(_timeslices)) {
        _it = next(_last); // try to resume from previous position
    } else {
        _it = begin(_timeslices); // only once at the beginning
    }

    StorableTimeslice *ts_p;

    if (_it != end(_timeslices)) {
        _last = _it; // save position for next time
        // move out of the map onto the heap, will be managed by a unique_ptr
        ts_p = new StorableTimeslice {std::move(_it->second)};
    } else {
        ts_p = nullptr;
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
