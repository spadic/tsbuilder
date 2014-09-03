#include "MicrosliceSource.hpp"
// fles_ipc
#include "MicrosliceContainer.hpp" // needs microslice branch

namespace fles {

MicrosliceSource::MicrosliceSource(uint16_t eq_id, uint8_t sys_id,
                                   uint8_t sys_ver, uint64_t start_index)
: _eq_id {eq_id},
  _sys_id {sys_id},
  _sys_ver {sys_ver},
  start_index {start_index}
{
};

void MicrosliceSource::add(std::vector<uint8_t> content)
{
    auto mc_index = start_index + size();
    _microslices.emplace_back(_desc(mc_index), std::move(content));
}

MicrosliceContainer MicrosliceSource::get(uint64_t mc_index)
{
    if (mc_index >= start_index && mc_index < start_index + size()) {
        return _microslices[mc_index - start_index];
    } else {
        return {_desc(mc_index), {}};
    }
}

size_t MicrosliceSource::size()
{
    return _microslices.size();
}

// create the descriptor for an empty microslice with given index
MicrosliceDescriptor MicrosliceSource::_desc(uint64_t index)
{
    return {0xDD, // hdr_id
            0x01, // hdr_ver
            _eq_id,
            0, // flags
            _sys_id,
            _sys_ver,
            index,
            0, // crc
            0, // size -> set by MicrosliceContainer constructor
            0 // offset -> set by StorableTimeslice::append_microslice
    };
}

}
