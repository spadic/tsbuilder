TimesliceEmulator::TimesliceEmulator(size_t ts_len, uint64_t start_index)
: _ts_len {ts_len}, _start_idx {start_index}
{
}

fles::StorableTimeslice *do_get()
{
    if (_last != end(_timeslices)) {
        _it = next(_last); // try to resume from previous position
    } else {
        _it = begin(_timeslices); // only once at the beginning
    }
    if (_it != end(_timeslices)) {
        _last = _it; // save position for next time
        // need to make a copy on the heap, to not let the unique_ptr
        // destroy the contents of the map
        return new fles::StorableTimeslice {std::move(it->second)};
    }
}

void TimesliceEmulator::add_microslices(const MicrosliceSource& mc_source)
{
    auto ts_index = 0;//TODO
    auto& ts = _timeslices.emplace(ts_index, _ts_len).first->second;
    auto comp_index = ts.append_component(_ts_len, ts_index);
    auto mc_index = ts_index * _ts_len;
    for (size_t i = 0; i < _ts_len; i++) {
        auto mc = mc_source.get(mc_index + i);
        ts.append_microslice(comp_index, i, mc);
    }
}
