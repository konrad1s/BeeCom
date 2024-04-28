#pragma once

#include <cstdint>
#include <functional>

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
        uint8_t payload[MAX_PAYLOAD_SIZE] = {};
        void reset() { *this = Packet(); }
    };


    using CRCFunction = std::function<uint16_t(const PacketHeader &, const uint8_t *, size_t)>;
    using SendFunction = std::function<void(const Packet &)>;
    using PacketHandler = std::function<void(const Packet &, bool crcValid, SendFunction send)>;
    using InvalidPacketHandler = std::function<void(SendFunction send)>;
}
