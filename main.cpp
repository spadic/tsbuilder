#include "StorableTimeslice.hpp"
#include "TimesliceOutputArchive.hpp"

int main()
{

const size_t NUM_TIMESLICES {10};
const size_t NUM_COMPONENTS {1};
const size_t NUM_MICROSLICES {100};

const uint8_t SYS_ID {0x40}; // TRD
const uint8_t SYS_VER {0x01};

const size_t SIZE {20}; // TODO

fles::TimesliceOutputArchive ar {"output.tsa"};

for (uint64_t ts_index {0}; ts_index < NUM_TIMESLICES; ts_index++) {
    fles::StorableTimeslice ts {NUM_MICROSLICES};

    for (uint16_t comp_index {0}; comp_index < NUM_COMPONENTS; comp_index++) {
        ts.append_component(NUM_MICROSLICES, ts_index);

        for (uint64_t mc_index {0}; mc_index < NUM_MICROSLICES; mc_index++) {
            auto mc_size = SIZE + ((~SIZE + 1) % 8); // round up to multiple of 8
            fles::MicrosliceDescriptor mc_desc {
                0xDD, // hdr_id
                0x01, // hdr_ver
                comp_index, // eq_id
                0, // flags
                SYS_ID, // sys_id TODO
                SYS_VER, // sys_ver TODO
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
