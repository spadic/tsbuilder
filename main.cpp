#include "StorableTimeslice.hpp"
#include "TimesliceOutputArchive.hpp"
#include <unordered_map>

int main()
{

const size_t NUM_TIMESLICES {10};
const size_t NUM_COMPONENTS {1};
const size_t NUM_MICROSLICES {100};

const uint16_t EQ_ID[NUM_COMPONENTS] {1};
const uint8_t SYS_ID[NUM_COMPONENTS] {0x40}; // TRD
const uint8_t SYS_VER[NUM_COMPONENTS] {0x01};

const size_t SIZE {20}; // TODO

fles::TimesliceOutputArchive ar {"output.tsa"};

for (uint64_t ts_index {0}; ts_index < NUM_TIMESLICES; ts_index++) {
    fles::StorableTimeslice ts {NUM_MICROSLICES};

// one timeslice per ts_index
std::unordered_map<uint64_t, fles::StorableTimeslice> timeslices;

std::unordered_map<
    uint16_t, // eq_id
    std::unordered_map<
        uint64_t, // mc_index
        std::pair<fles::MicrosliceDescriptor, uint8_t *> // desc, content
    >
> microslices;

void add_component_from_eq_id(uint16_t eq_id, uint64_t mc_index_start)
{
    uint64_t ts_index = mc_index_start/TIMESLICE_LENGTH;
    // get timeslice corresponding to ts_index, create if necessary
    auto& ts = (*timeslices.emplace(ts_index, TIMESLICE_LENGTH).first).second;
    uint32_t comp_index = ts.append_component(TIMESLICE_LENGTH, ts_index);
    uint64_t mc_index {mc_index_start};
    for (uint64_t mc_index {mc_index_start};
            mc_index < mc_index_start + TIMESLICE_LENGTH; mc_index++) {
        auto& mc_item = microslices[eq_id][comp_index];
        ts.append_microslice(comp_index, mc_index, mc_item.first, mc_item.second);
    }
}

    uint16_t comp_index {0};
    while (comp_index < NUM_COMPONENTS) {
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
    }
    ar.write(ts);
}

return 0;

} // main
