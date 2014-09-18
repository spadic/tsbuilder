#include "MicrosliceSource.hpp"
#include "TimesliceBuilder.hpp"

#include "TimesliceOutputArchive.hpp"
#include "MicrosliceContents.hpp"

#include <zmq.hpp>

//==== constants =====================================================

const uint16_t QUIT {0xFFFF};
const uint16_t NEXT_MC {0xFFFE};
const uint16_t ADD_SRC {0xFFFD};
const char *IPC_ADDRESS = "ipc:///tmp/tsbuilder.ipc";

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

        if (action == ADD_SRC) {
            // [ADD_SRC, eq_id, sys_id, sys_ver, mc_start_idx]
            if (n < 5) { continue; }
            auto eq_id = *data++;
            auto sys_id = uint8_t (*data++);
            auto sys_ver = uint8_t (*data++);
            auto mc_start_idx = *data++;
            sources.emplace_back(eq_id, sys_id, sys_ver, mc_start_idx);
            contents.emplace_back();
            continue;
        }
        if (action == NEXT_MC) {
            // [NEXT_MC, src_idx]
            if (n < 2) { continue; }
            auto src_idx = *data++;
            if (!(src_idx < sources.size())) { continue; }
            add_microslice_contents(sources[src_idx], contents[src_idx]);
            contents[src_idx] = flib_dpb::MicrosliceContents {};
        }
        if (action == QUIT) {
            // [QUIT]
            for (size_t i = 0; i < sources.size(); i++) {
                add_microslice_contents(sources[i], contents[i]);
            }
            break;
        }

        // default action: add new DTM
        // [---, src_idx, cbm_addr, d0, d1, ...]
        if (n < 3) { continue; }
        auto src_idx = *data++;
        if (!(src_idx < contents.size())) { continue; }
        auto cbm_addr = *data++;
        contents[src_idx].add_dtm({cbm_addr, data, n-3});
    }

    //---- after the data is accumulated: write timeslices to file ---

    auto bld = fles::TimesliceBuilder {10, 0}; // TODO from argv
    for (auto mc_src : sources) {
        bld.add_microslices(mc_src);
    }

    fles::TimesliceOutputArchive ar {"output.tsa"}; // TODO from argv
    while (auto ts_up = bld.get()) {
        ar.write(*ts_up);
    }
}
