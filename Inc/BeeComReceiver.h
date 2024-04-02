#pragma once

#include "BeeComTypes.h"
#include <cstring>
#include <functional>

namespace beecom
{
    class Receiver
    {
    public:
        Receiver(CRCFunction crcFunc, uint8_t sop, SendFunction sendFunc,
                 InvalidPacketHandler invalidHandler = [](SendFunction) {})
            : crcCalculation(crcFunc), sopValue(sop), sendFunction(sendFunc), invalidPacketHandler(invalidHandler)
        {
        }

        void Deserialize(const uint8_t *data, size_t size);
        void setPacketHandler(PacketHandler handler)
        {
            packetHandler = handler;
        }

    private:
        Packet packet;
        size_t payloadCounter = 0;
        PacketState state = PacketState::SOP_WAITING;
        PacketHandler packetHandler;
        CRCFunction crcCalculation;
        SendFunction sendFunction;
        InvalidPacketHandler invalidPacketHandler;
        uint8_t sopValue;

        void handleStateChange(uint8_t byte);
        void handleSOPWaiting(uint8_t byte);
        void handleTypeWaiting(uint8_t byte);
        void handleLengthLsbWaiting(uint8_t byte);
        void handleLengthMsbWaiting(uint8_t byte);
        void handleCRCLowWaiting(uint8_t byte);
        void handleCRCHighWaiting(uint8_t byte);
        void handleGettingPayload(uint8_t byte);
        void processCompletePacket();
        bool validateCRC() const;
        void resetState();
    };
}
