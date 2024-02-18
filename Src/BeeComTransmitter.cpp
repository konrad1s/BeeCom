#include <cstring>
#include "BeeComTransmitter.h"

namespace beecom
{
    size_t Transmitter::Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const
    {
        const size_t requiredSize = calculateRequiredSize(packet);
        /* TODO: add assert */

        size_t size = 0U;

        buffer[size++] = packet.header.sop;
        buffer[size++] = packet.header.type;
        buffer[size++] = packet.header.length;

        for (size_t i = 0U; i < packet.header.length; ++i)
        {
            buffer[size++] = packet.payload[i];
        }

        uint16_t crc = crcCalculation(packet.header, packet.payload, packet.header.length);
        buffer[size++] = static_cast<uint8_t>(crc & 0xFFU);
        buffer[size++] = static_cast<uint8_t>((crc >> 8U) & 0xFFU);

        return size;
    }

    size_t Transmitter::calculateRequiredSize(const Packet &packet) const
    {
        return sizeof(packet.header.sop) + sizeof(packet.header.type) +
               sizeof(packet.header.length) + packet.header.length + sizeof(packet.header.crc);
    }

} // namespace beecom
