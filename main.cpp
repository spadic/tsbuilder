#include "StorableTimeslice.hpp"
#include "TimesliceOutputArchive.hpp"
#include <unordered_map>
#include <vector>
#include <cstdio>


//-------------------

struct MicrosliceContainer {
    fles::MicrosliceDescriptor desc;
    uint8_t *content;
};

struct MicrosliceSource {
    MicrosliceSource(uint16_t eq_id, uint8_t sys_id, uint8_t sys_ver)
        : _eq_id {eq_id}, _sys_id {sys_id}, _sys_ver {sys_ver} {};

    void add(uint64_t mc_index, std::vector<uint8_t> content)
    {
        // what
        MicrosliceContainer mc {_desc(mc_index, content.size()),
                                content.data()};
        // where
        auto it = _microslices.find(mc_index);
        if (it != end(_microslices)) {
            it->second = mc; // overwrite existing
        } else {
            _microslices.insert(it, {mc_index, mc}); // add new
        }
    };

    MicrosliceContainer get(uint64_t mc_index)
    {
        auto it = _microslices.find(mc_index);
        if (it != end(_microslices)) {
            return it->second;
        } else {
            auto desc = _desc(mc_index, 0);
            return {desc, nullptr};
        }
    };

private:
    uint16_t _eq_id;
    uint8_t _sys_id, _sys_ver;
    fles::MicrosliceDescriptor _desc(uint64_t index, uint32_t size)
    {
        return {0xDD, // hdr_id
                0x01, // hdr_ver
                _eq_id,
                0, // flags
                _sys_id,
                _sys_ver,
                index,
                0, // crc
                size};
    };
    std::unordered_map<uint64_t, MicrosliceContainer> _microslices;
};

// one MicrosliceSource per eq_id
using MicrosliceSourceMap = std::unordered_map<uint16_t, MicrosliceSource>;

// one timeslice per timeslice index
std::unordered_map<uint64_t, fles::StorableTimeslice> timeslices;

const size_t TIMESLICE_LENGTH {10}; // microslices per timeslice

void add_component_from_eq_id(MicrosliceSourceMap mc_sources,
                              uint16_t eq_id, uint64_t ts_index)
{
    // get timeslice corresponding to timeslice index, create if necessary
    // roughly equivalent to the following Python code:
    //   if ts_index not in timeslices:
    //       timeslices[ts_index] = Timeslice(TIMESLICE_LENGTH)
    //   ts = timeslices[ts_index]
    // or simply:
    //   ts = timeslices.setdefault(ts_index, Timeslice(TIMESLICE_LENGTH))
    auto& ts = timeslices.emplace(ts_index, TIMESLICE_LENGTH).first->second;

    // get component index by creating a new component
    auto c = ts.append_component(TIMESLICE_LENGTH, ts_index);

    // get {mc_index: (desc, content), ...} object for this eq_id
    auto& mc_source = mc_sources.at(eq_id);

    // iterate over microslice interval corresponding to timeslice index
    auto m = ts_index * TIMESLICE_LENGTH;
    for (auto i = 0; i < TIMESLICE_LENGTH; i++) {
        auto mc = mc_source.get(m+i);
        ts.append_microslice(c, m+i, mc.desc, mc.content);
    }
}

//-------------------

const size_t NUM_COMPONENTS {1};
const size_t NUM_MICROSLICES {100}; // total, over all timeslices

const uint16_t EQ_ID[NUM_COMPONENTS] {1};
const uint8_t SYS_ID[NUM_COMPONENTS] {0x40}; // TRD
const uint8_t SYS_VER[NUM_COMPONENTS] {0x01};

const size_t SIZE {20}; // TODO

fles::TimesliceOutputArchive ar {"output.tsa"};

int main()
{

for (uint64_t ts_index {0}; ts_index < NUM_MICROSLICES/TIMESLICE_LENGTH; ts_index++) {
    fles::StorableTimeslice ts {TIMESLICE_LENGTH};

    uint32_t comp_index;
    do {
        comp_index = ts.append_component(NUM_MICROSLICES, ts_index);

        for (uint64_t mc_index {0}; mc_index < NUM_MICROSLICES; mc_index++) {
            uint32_t mc_size = SIZE + ((~SIZE + 1) % 8); // round up to multiple of 8
            fles::MicrosliceDescriptor mc_desc {
                0xDD, // hdr_id
                0x01, // hdr_ver
                EQ_ID[comp_index],
                0, // flags
                SYS_ID[comp_index],
                SYS_VER[comp_index],
                mc_index, // idx TODO
                0, // crc
                mc_size, // size TODO
                0 // offset -> overwritten by StorableTimeslice::append_microslice
            };
            uint8_t mc_content[mc_size]; // TODO
            for (uint8_t i {0}; i < mc_size; i++) {
                mc_content[i] = i;
            }
            ts.append_microslice(comp_index, mc_index, mc_desc, mc_content);
        }
    } while (comp_index+1 < NUM_COMPONENTS);
    ar.write(ts);
}

} // main
