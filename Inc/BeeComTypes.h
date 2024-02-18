#ifndef BEECOMPACKET_H_
#define BEECOMPACKET_H_

#include <cstdint>
#include <cassert>
#include <cstddef>

namespace beecom
{
    constexpr uint8_t MAX_PAYLOAD_SIZE = 200U;
    constexpr uint8_t SOP_VALUE = 0xA5U;

    struct PacketHeader
    {
        uint8_t sop = SOP_VALUE;
        uint8_t type = 0U;
        uint8_t length = 0U;
        uint16_t crc = 0U;
    };

    struct Packet
    {
        PacketHeader header;
        uint8_t payload[MAX_PAYLOAD_SIZE] = {};
        void reset() { *this = Packet(); }
    };

    enum class PacketState
    {
        SOP_WAITING,
        TYPE_WAITING,
        LEN_WAITING,
        GETTING_PAYLOAD,
        CRC_LSB_WAITING,
        CRC_MSB_WAITING,
        PACKET_RECEIVED
    };

    using CRCFunction = uint16_t (*)(const PacketHeader &header, const uint8_t *payload, size_t payloadLength);
    using SendFunction = void (*)(const Packet &);
    using PacketHandler = void (*)(const Packet &packet, bool crcValid, SendFunction send);

} // namespace beecom

#endif /* BEECOMPACKET_H_ */
