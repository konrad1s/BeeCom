#pragma once

#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include <cstring>

namespace beecom
{
    class Serializer
    {
    public:
        explicit Serializer(CRCFunction crcFunc = CalculateFullPacketCRC)
            : crcCalculation(crcFunc) {}

        size_t Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const;
        size_t SerializeHeader(const Packet &packet, uint8_t *buffer) const;
        size_t SerializeCRC(const Packet &packet, uint8_t *buffer) const;

    private:
        CRCFunction crcCalculation;
        size_t CalculateRequiredSize(const Packet &packet) const;
    };
}
