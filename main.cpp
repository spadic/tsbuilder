#include "StorableTimeslice.hpp"
#include "TimesliceOutputArchive.hpp"

int main()
{

const int NUM_TIMESLICES {10};
const int NUM_COMPONENTS {1};
const int NUM_MICROSLICES {100};

TimesliceOutputArchive ar {"output.tsa"};

int ts_index;
for (ts_index {0}; ts_index < MAX_TS_INDEX; ts_index++) {
    StorableTimeslice ts {NUM_MICROSLICES};

    int comp_index;
    for (comp_index {0}; comp_index < NUM_COMPONENTS; comp_index++) {
        ts.append_component(NUM_MICROSLICES, ts_index)

        int mc_index {0};
        while (mc_index < NUM_MICROSLICES) {
            MicrosliceDescriptor mc_desc; // TODO
            uint8_t *mc_content; // TODO
            ts.append_microslice(comp_index, mc_index, mc_desc, mc_content);
        }
    }
    ar.write(&ts);
}

return 0;

} // main
