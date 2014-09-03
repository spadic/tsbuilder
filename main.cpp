#include "MicrosliceSource.hpp"
#include "TimesliceBuilder.hpp"
#include "TimesliceOutputArchive.hpp"
#include <cstdio>

//--------------------------------------------------------------------

const size_t NUM_COMPONENTS {1};
const size_t NUM_MICROSLICES {100}; // total, over all timeslices

const uint16_t EQ_ID[NUM_COMPONENTS] {1};
const uint8_t SYS_ID[NUM_COMPONENTS] {0x40}; // TRD
const uint8_t SYS_VER[NUM_COMPONENTS] {0x01};

const size_t SIZE {20}; // TODO

fles::TimesliceOutputArchive ar {"output.tsa"};

int main()
{
    auto bld = TimesliceBuilder {10, 15};
    auto mc_src = fles::MicrosliceSource {1, 0x40, 0x01, 8};
    for (size_t i = 0; i < 5000; i++) {
        auto v = std::vector<uint8_t> (7, i);
        mc_src.add(v);
    }
    bld.add_microslices(mc_src);
    while (auto ts_up = bld.get()) {
        printf("got timeslice @ %p\n", &(*ts_up));
        ar.write(*ts_up);
    }

/*
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
                mc_index, // idx
                0, // crc
                0, // size
                0 // offset -> overwritten by StorableTimeslice::append_microslice
            };
            auto mc_content = std::vector<uint8_t> {};
            for (uint8_t i {0}; i < mc_size; i++) {
                mc_content.push_back(i);
            }
            auto mc = fles::MicrosliceContainer {mc_desc, mc_content};
            ts.append_microslice(comp_index, mc_index, mc);
        }
    } while (comp_index+1 < NUM_COMPONENTS);
    ar.write(ts);
}
*/

} // main
