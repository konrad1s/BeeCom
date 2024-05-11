#pragma once

#include <cstdint>
#include <cstddef>

namespace beecom
{
    constexpr uint16_t MAX_PAYLOAD_SIZE = 2048U;

    struct PacketHeader
    {
        uint8_t sop = 0U;
        uint8_t type = 0U;
        uint16_t length = 0U;
        uint16_t crc = 0U;
    };

    struct Packet
    {
        PacketHeader header;
        uint8_t *payload;
        void reset() { *this = Packet(); }
    };


    typedef uint16_t (*CRCFunction)(const PacketHeader &, const uint8_t *, size_t);
    typedef void (*SendFunction)(const Packet &);
    typedef void (*PacketHandler)(const Packet &, bool crcValid, SendFunction send);
    typedef void (*InvalidPacketHandler)(SendFunction send);
}
