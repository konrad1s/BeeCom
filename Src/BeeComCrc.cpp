#include "BeeComCrc.h"

namespace beecom
{
    uint16_t UpdateCRC(uint16_t currentCRC, uint8_t dataByte)
    {
        return (currentCRC << 8) ^ lookupTable[((currentCRC >> 8) ^ dataByte) & 0xFF];
    }

    uint16_t CalculateFullPacketCRC(const PacketHeader &header, const uint8_t *payload, size_t payloadLength)
    {
        static constexpr uint16_t initialCRCValue = 0x1D0F;
        uint16_t crc = initialCRCValue;

        crc = UpdateCRC(crc, header.sop);
        crc = UpdateCRC(crc, header.type);
        crc = UpdateCRC(crc, header.length & 0xFF);
        crc = UpdateCRC(crc, header.length >> 8);

        for (size_t i = 0; i < payloadLength; ++i)
        {
            crc = UpdateCRC(crc, payload[i]);
        }

        return crc;
    }

} // namespace beecom
