#include "TimesliceBuilder.hpp"
#include "MicrosliceSource.hpp"
#include <cstdio>

TimesliceBuilder::TimesliceBuilder(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
    _it = end(_timeslices);
    _last = end(_timeslices);
}

std::unique_ptr<fles::StorableTimeslice> TimesliceBuilder::get()
{
    printf("get_timeslice()\n");
    if (_last != end(_timeslices)) {
        printf("_it = next(_last)\n");
        _it = next(_last); // try to resume from previous position
    } else {
        printf("_it = begin(_timeslices)\n");
        _it = begin(_timeslices); // only once at the beginning
        printf("after _it = begin(_timeslices)\n");
    }

    fles::StorableTimeslice *ts_p;

    if (_it != end(_timeslices)) {
        printf("_last = _it\n");
        _last = _it; // save position for next time
        // move out of the map onto the heap, will be managed by a unique_ptr
        //return new fles::StorableTimeslice {std::move(_it->second)};
        printf("_it points to %p\n", &_it->second);
        ts_p = new fles::StorableTimeslice {std::move(_it->second)};
    } else {
        ts_p = nullptr;
    }
    return std::unique_ptr<fles::StorableTimeslice> {ts_p};
}

void TimesliceBuilder::add_microslices(fles::MicrosliceSource& mc_source)
{
    auto num_microslices = mc_source.size();
    printf("add %d microslices\n", num_microslices);
    for (auto ts_index = mc_source.start_index / _ts_len;
         ts_index * _ts_len < mc_source.start_index + num_microslices;
         ts_index++) {
        printf("ts_index = %d\n", ts_index);
        auto& ts = _timeslices.emplace(ts_index, _ts_len).first->second;
        auto comp_index = ts.append_component(_ts_len, ts_index);
        auto mc_index = ts_index * _ts_len;
        for (size_t i = 0; i < _ts_len; i++) {
            auto mc = mc_source.get(mc_index + i);
            ts.append_microslice(comp_index, i, mc);
        }
    }
}
