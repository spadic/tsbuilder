#include "MicrosliceContainer.hpp"
#include "MicrosliceSource.hpp"

namespace fles {

MicrosliceSource::MicrosliceSource(uint16_t eq_id, uint8_t sys_id, uint8_t sys_ver)
: _eq_id {eq_id},
  _sys_id {sys_id},
  _sys_ver {sys_ver}
{
};

void MicrosliceSource::add(uint64_t mc_index, std::vector<uint8_t> content)
{
    // what
    auto mc = MicrosliceContainer {_desc(mc_index), std::move(content)};
    // where
    auto it = _microslices.find(mc_index);
    if (it != end(_microslices)) {
        it->second = mc; // overwrite existing
    } else {
        _microslices.insert(it, {mc_index, mc}); // add new
    }
}

MicrosliceContainer MicrosliceSource::get(uint64_t mc_index)
{
    auto it = _microslices.find(mc_index);
    if (it != end(_microslices)) {
        return it->second;
    } else {
        return {_desc(mc_index), {}};
    }
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
