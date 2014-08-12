#include "StorableTimeslice.hpp"
#include "TimesliceOutputArchive.hpp"

int main()
{

const int NUM_TIMESLICES {10};
const int NUM_COMPONENTS {1};
const int NUM_MICROSLICES {100};

fles::TimesliceOutputArchive ar {"output.tsa"};

for (int ts_index {0}; ts_index < NUM_TIMESLICES; ts_index++) {
    fles::StorableTimeslice ts {NUM_MICROSLICES};

    for (int comp_index {0}; comp_index < NUM_COMPONENTS; comp_index++) {
        ts.append_component(NUM_MICROSLICES, ts_index);

        for (int mc_index {0}; mc_index < NUM_MICROSLICES; mc_index++) {
            fles::MicrosliceDescriptor mc_desc; // TODO
            uint8_t *mc_content; // TODO
            ts.append_microslice(comp_index, mc_index, mc_desc, mc_content);
        }
    }
    ar.write(ts);
}

return 0;

} // main
