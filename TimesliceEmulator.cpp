#include "TimesliceEmulator.hpp"
#include <cstdio>

TimesliceEmulator::TimesliceEmulator(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
    _it = end(_timeslices);
    _last = end(_timeslices);
}

fles::StorableTimeslice *TimesliceEmulator::get_timeslice()
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
    if (_it != end(_timeslices)) {
        printf("_last = _it\n");
        _last = _it; // save position for next time
        // need to make a copy on the heap, to not let the unique_ptr
        // destroy the contents of the map
        return &_it->second;
    } else {
        return nullptr;
    }
}

void TimesliceEmulator::add_microslices(fles::MicrosliceSource& mc_source)
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
