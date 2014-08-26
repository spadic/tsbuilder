#include "MicrosliceSource.hpp"

namespace fles {

MicrosliceSource::MicrosliceSource(uint16_t eq_id, uint8_t sys_id, uint8_t sys_ver)
: _eq_id {eq_id},
  _sys_id {sys_id},
  _sys_ver {sys_ver}
{
};

void MicrosliceSource::add(uint64_t mc_index, const std::vector<uint8_t>& content)
{
    // what
    auto mc = MicrosliceContainer {_desc(mc_index, content.size()),
                                   content.data()};
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
        auto desc = _desc(mc_index, 0);
        return {desc, nullptr};
    }
}

MicrosliceDescriptor MicrosliceSource::_desc(uint64_t index, uint32_t size)
{
    return {0xDD, // hdr_id
            0x01, // hdr_ver
            _eq_id,
            0, // flags
            _sys_id,
            _sys_ver,
            index,
            0, // crc
            size};
}

}
