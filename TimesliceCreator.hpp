#include "MicrosliceSource.hpp"
#include "TimesliceSource.hpp"

class TimesliceCreator : public fles::TimesliceSource {
public:
    void add_microslices(const MicrosliceSource& mc_source);

private:
    fles::StorableTimeslice do_get();
    std::map<uint64_t, fles::StorableTimeslice> _timeslices;
    decltype(_timeslices)::iterator _it, _last;
    size_t _ts_len;
    uint64_t _start_idx;
};

