#include "BeeComCrc.h"

namespace beecom {

void BeeComCrc::UpdateCRC(uint8_t dataByte)
{
    currentCrcValue = (currentCrcValue << 8) ^ lookupTable[((currentCrcValue >> 8) ^ dataByte) & 0xFF];
}

void BeeComCrc::UpdateCRC(const uint8_t* data, size_t length)
{
    for (size_t i = 0U; i < length; ++i)
    {
        UpdateCRC(data[i]);
    }
}

uint16_t BeeComCrc::CalculateFullPacketCRC(const PacketHeader& header, const uint8_t* payload, size_t payloadLength)
{
    UpdateCRC(header.sop);
    UpdateCRC(header.type);
    UpdateCRC(header.length & 0xFF);
    UpdateCRC(header.length >> 8);
    UpdateCRC(payload, payloadLength);

    return currentCrcValue;
}

uint16_t BeeComCrc::GetCRC() const
{
    return currentCrcValue;
}

} // namespace beecom
