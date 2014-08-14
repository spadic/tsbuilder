//! \author Michael Krieger

#include <iostream>
#include <cstdio>
#include "TimesliceInputArchive.hpp"

void print_descriptor(const fles::MicrosliceDescriptor& desc)
{
    /*
    uint8_t hdr_id;  ///< Header format identifier (0xDD)
    uint8_t hdr_ver; ///< Header format version (0x01)
    uint16_t eq_id;  ///< Equipment identifier
    uint16_t flags;  ///< Status and error flags
    uint8_t sys_id;  ///< Subsystem identifier
    uint8_t sys_ver; ///< Subsystem format version
    uint64_t idx;    ///< Microslice index
    uint32_t crc;    ///< CRC32 checksum
    uint32_t size;   ///< Content size (bytes)
    uint64_t offset; ///< Offset in event buffer (bytes)
    */
    printf("hdr_id: 0x%02X\n", desc.hdr_id);
    printf("hdr_ver: 0x%02X\n", desc.hdr_ver);
    printf("eq_id: 0x%04X\n", desc.eq_id);
    printf("flags: 0x%04X\n", desc.flags);
    printf("sys_id: 0x%02X\n", desc.sys_id);
    printf("sys_ver: 0x%02X\n", desc.sys_ver);
    printf("idx: %d\n", desc.idx);
    printf("crc: 0x%08X\n", desc.crc);
    printf("size: 0x%X\n", desc.size);
    printf("offset: 0x%X\n", desc.offset);
}


void check_microslice(const fles::Timeslice& ts, size_t comp_idx, size_t mc_idx)
{
    auto& desc = ts.descriptor(comp_idx, mc_idx);
    printf("---- microslice %d ----\n", mc_idx);
    print_descriptor(desc);
}

void read_timeslice_archive(const std::string& filename)
{
    fles::TimesliceInputArchive ar {filename};

    while (auto p_ts = ar.get()) {
        auto& ts = *p_ts;
        size_t nc = ts.num_components();
        printf("got timeslice with %d components\n", nc);
        for (size_t c {0}; c < nc; c++) {
            size_t nm = ts.num_microslices(c);
            printf("component %d has %d microslices\n", c, nm);
            for (size_t m {0}; m < nm; m++) {
                check_microslice(ts, c, m);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    if (!(argc > 1)) {
        std::cout << "Please specify path to .tsa file." << std::endl;
        return 1;
    }
    std::string filename;
    try {
        filename = argv[1];
        read_timeslice_archive(filename);
        return 0;
    }
    catch (boost::archive::archive_exception& e) {
        std::cout << "Could not read \"" << filename <<
                     "\" as Timeslice archive." << std::endl;
        return 1;
    }
}
