#include "MicrosliceDescriptor.hpp"
#include <unordered_map>
#include <vector>

namespace fles {

struct MicrosliceContainer {
    MicrosliceDescriptor desc;
    uint8_t *content;
};

class MicrosliceSource {
public:
    MicrosliceSource(uint16_t eq_id, uint8_t sys_id, uint8_t sys_ver);
    void add(uint64_t mc_index, std::vector<uint8_t> content);
    MicrosliceContainer get(uint64_t mc_index);

private:
    uint16_t _eq_id;
    uint8_t _sys_id, _sys_ver;
    MicrosliceDescriptor _desc(uint64_t index, uint32_t size);
    std::unordered_map<uint64_t, MicrosliceContainer> _microslices;
};

}
