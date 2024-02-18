#ifndef BEECOM_TRANSMITTER_H_
#define BEECOM_TRANSMITTER_H_

#include <cstdint>
#include <cassert>
#include <cstddef>
#include "BeeComTypes.h"
#include "BeeComCrc.h"

namespace beecom
{
    class Transmitter
    {
    public:
        Transmitter(CRCFunction crcFunc = calculateFullPacketCRC)
            : crcCalculation(crcFunc) {}

        size_t Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const;

    private:
        CRCFunction crcCalculation;
        size_t calculateRequiredSize(const Packet &packet) const;
    };

} // namespace beecom

#endif /* BEECOM_TRANSMITTER_H_ */
