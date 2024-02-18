#ifndef BEECOM_RECEIVER_H_
#define BEECOM_RECEIVER_H_

#include <cstdint>
#include <cassert>
#include <cstddef>
#include "BeeComTypes.h"
#include "BeeComCrc.h"

namespace beecom
{
    class Receiver
    {
    public:
        Receiver(PacketHandler callback, CRCFunction crcFunc = calculateFullPacketCRC);
        size_t Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const;
        void Deserialize(const uint8_t *data, size_t size);
        void setSendFunction(SendFunction sendFunc)
        {
            this->sendFunction = sendFunc;
        }

    private:
        Packet packet;
        size_t payloadCounter = 0;
        PacketState state = PacketState::SOP_WAITING;
        PacketHandler packetHandler;
        CRCFunction crcCalculation;
        SendFunction sendFunction;

        void handleStateChange(uint8_t byte);
        void handleSOPWaiting(uint8_t byte);
        void handleTypeWaiting(uint8_t byte);
        void handleLengthWaiting(uint8_t byte);
        void handleCRCLowWaiting(uint8_t byte);
        void handleCRCHighWaiting(uint8_t byte);
        void handleGettingPayload(uint8_t byte);
        void processCompletePacket();
        bool validateCRC() const;
        void resetState();
    };

} // namespace beecom

#endif /* BEECOM_RECEIVER_H_ */
