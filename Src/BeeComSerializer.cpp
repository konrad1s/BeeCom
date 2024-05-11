#include <cstring>
#include "BeeComSerializer.h"

namespace beecom {
size_t Serializer::Serialize(const Packet& packet, uint8_t* buffer, size_t bufferSize) const
{
    if (bufferSize < CalculateRequiredSize(packet))
    {
        return 0;
    }

    size_t offset = SerializeHeader(packet, buffer);
    memcpy(buffer + offset, packet.payload, packet.header.length);
    offset += packet.header.length;
    offset += SerializeCRC(packet, buffer + offset);

    return offset;
}

size_t Serializer::SerializeHeader(const Packet& packet, uint8_t* buffer) const
{
    buffer[0] = packet.header.sop;
    buffer[1] = packet.header.type;
    buffer[2] = static_cast<uint8_t>(packet.header.length & 0xFFU);
    buffer[3] = static_cast<uint8_t>((packet.header.length >> 8U) & 0xFFU);

    return 4U;
}

size_t Serializer::SerializeCRC(const Packet& packet, uint8_t* buffer) const
{
    uint16_t crc = crcCalculation(packet.header, packet.payload, packet.header.length);
    buffer[0] = static_cast<uint8_t>(crc & 0xFFU);
    buffer[1] = static_cast<uint8_t>((crc >> 8U) & 0xFFU);

    return 2U;
}

size_t Serializer::CalculateRequiredSize(const Packet& packet) const
{
    return sizeof(packet.header.sop) + sizeof(packet.header.type) + sizeof(packet.header.length) + packet.header.length
        + sizeof(packet.header.crc);
}

} // namespace beecom
