#include "MicrosliceSource.hpp"
#include "TimesliceBuilder.hpp"

#include "TimesliceOutputArchive.hpp"
#include "MicrosliceContents.hpp"

#include <zmq.hpp>
#include <cstdio>

//==== constants =====================================================

const uint16_t QUIT {0xFFFF};
const uint16_t NEXT_MC {0xFFFE};
const uint16_t ADD_SRC {0xFFFD};
const char *IPC_ADDRESS = "ipc:///tmp/tsbuilder.ipc";

// TODO from argv
const char *OUTPUT_TSA = "output.tsa";
const size_t TS_LEN = 10;
const uint64_t TS_START_IDX = 0;

//==== helper function ===============================================

void add_microslice_contents(fles::MicrosliceSource& src,
                             const flib_dpb::MicrosliceContents& mc)
{
    auto contents = mc.raw();
    auto start_p = reinterpret_cast<uint8_t *>(contents.data());
    auto end_p = start_p + contents.size() * sizeof(*contents.data());
    src.add({start_p, end_p});
}

//==== main ==========================================================

int main()
{
    auto sources = std::vector<fles::MicrosliceSource> {};
    auto contents = std::vector<flib_dpb::MicrosliceContents> {};

    auto context = zmq::context_t {1};
    auto socket = zmq::socket_t {context, ZMQ_PULL};
    socket.bind(IPC_ADDRESS);
    printf("bound ZMQ_PULL socket to %s\n", IPC_ADDRESS);

    //---- main loop: read data from ZMQ socket ----------------------

    auto m = zmq::message_t {};
    while (true) {
        // wait for message and read as list of 16 bit words
        socket.recv(&m);
        auto data = static_cast<uint16_t *>(m.data());
        auto n = m.size() / sizeof(*data);

        // first word determines the action
        if (n < 1) { continue; }
        auto action = *data++;

        //---- [ADD_SRC, eq_id, sys_id, sys_ver, mc_start_idx] -------
        if (action == ADD_SRC) {
            if (n < 5) {
                printf("not enough arguments for ADD_SRC\n");
                continue;
            }
            auto eq_id = *data++;
            auto sys_id = uint8_t (*data++);
            auto sys_ver = uint8_t (*data++);
            auto mc_start_idx = *data++;
            sources.emplace_back(eq_id, sys_id, sys_ver, mc_start_idx);
            contents.emplace_back();
            printf("added MicrosliceSource:\n"
                   "  eq_id: 0x%04X\n"
                   "  sys_id: 0x%02X  sys_ver: 0x%02X\n"
                   "  mc_start_index: %d\n",
                   eq_id, sys_id, sys_ver, mc_start_idx);
            continue;
        }
        //---- [NEXT_MC, src_idx] ------------------------------------
        if (action == NEXT_MC) {
            if (n < 2) {
                printf("not enough arguments for NEXT_MC\n");
                continue;
            }
            auto src_idx = *data++;
            auto num_src = sources.size();
            if (!(src_idx < num_src)) {
                printf("invalid index %d (there are %d MicrosliceSources)\n",
                       src_idx, num_src);
                continue;
            }
            add_microslice_contents(sources[src_idx], contents[src_idx]);
            contents[src_idx] = flib_dpb::MicrosliceContents {};
        }
        //---- [QUIT] ------------------------------------------------
        if (action == QUIT) {
            for (size_t i = 0; i < sources.size(); i++) {
                add_microslice_contents(sources[i], contents[i]);
            }
            printf("quit.\n");
            break;
        }

        //---- default: [---, src_idx, cbm_addr, d0, d1, ...] --------
        if (n < 3) {
            printf("not enough arguments for ADD_DTM\n");
            continue;
        }
        auto src_idx = *data++;
        auto num_src = sources.size();
        if (!(src_idx < num_src)) {
            printf("invalid index %d (there are %d MicrosliceSources)\n",
                   src_idx, num_src);
            continue;
        }
        auto cbm_addr = *data++;
        auto num_words = n-3;
        contents[src_idx].add_dtm({cbm_addr, data, num_words});
        printf("added DTM to MicrosliceSource #%d\n"
               "  cbm_addr: %04X  num. words: %d\n",
               src_idx, cbm_addr, num_words);
    }

    //---- after the data is accumulated: write timeslices to file ---

    auto bld = fles::TimesliceBuilder {TS_LEN, TS_START_IDX};
    for (auto mc_src : sources) {
        bld.add_microslices(mc_src);
        printf("added %d microslices to TimesliceBuilder\n", mc_src.size());
    }

    fles::TimesliceOutputArchive ar {OUTPUT_TSA};
    printf("opened output archive %s\n", OUTPUT_TSA);
    while (auto ts_up = bld.get()) {
        ar.write(*ts_up);
        printf("wrote timeslice to archive\n");
    }
}
