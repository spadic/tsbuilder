// local
#include "TimesliceBuilder.hpp"
// fles_ipc
#include "MicrosliceSource.hpp" // needs microslice branch
#include "TimesliceOutputArchive.hpp"

int main()
{
    auto mc_src = fles::MicrosliceSource {1, 0x40, 0x01, 8};
    for (size_t i = 0; i < 5000; i++) {
        auto v = std::vector<uint8_t> (7, i);
        mc_src.add(v);
    }

    auto bld = TimesliceBuilder {10, 15};
    bld.add_microslices(mc_src);

    fles::TimesliceOutputArchive ar {"output.tsa"};
    while (auto ts_up = bld.get()) {
        ar.write(*ts_up);
    }
}
