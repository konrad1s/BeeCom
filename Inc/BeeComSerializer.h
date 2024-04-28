#pragma once

#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include <cstring>

namespace beecom
{
    class Serializer
    {
    public:
        explicit Serializer(CRCFunction crcFunc = calculateFullPacketCRC)
            : crcCalculation(crcFunc) {}

        size_t Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const;

    private:
        CRCFunction crcCalculation;
        size_t calculateRequiredSize(const Packet &packet) const;
    };
}
