#include <cstring>
#include "BeeComSerializer.h"

namespace beecom
{
    size_t Serializer::Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const
    {
        if (bufferSize < calculateRequiredSize(packet))
        {
            return 0;
        }

        size_t size = 0U;

        buffer[size++] = packet.header.sop;
        buffer[size++] = packet.header.type;
        buffer[size++] = static_cast<uint8_t>(packet.header.length & 0xFFU);
        buffer[size++] = static_cast<uint8_t>((packet.header.length >> 8U) & 0xFFU);

        for (size_t i = 0U; i < packet.header.length; ++i)
        {
            buffer[size++] = packet.payload[i];
        }

        uint16_t crc = crcCalculation(packet.header, packet.payload, packet.header.length);
        buffer[size++] = static_cast<uint8_t>(crc & 0xFFU);
        buffer[size++] = static_cast<uint8_t>((crc >> 8U) & 0xFFU);

        return size;
    }

    size_t Serializer::calculateRequiredSize(const Packet &packet) const
    {
        return sizeof(packet.header.sop) + sizeof(packet.header.type) +
               sizeof(packet.header.length) + packet.header.length +
               sizeof(packet.header.crc);
    }

} 
