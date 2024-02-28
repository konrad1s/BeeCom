#pragma once

#include "BeeComTypes.h"
#include <cstring> // For memcpy

namespace beecom
{
    class Receiver
    {
    public:
        Receiver(PacketHandler callback, CRCFunction crcFunc, uint8_t sop, SendFunction sendFunc)
            : packetHandler(callback), crcCalculation(crcFunc), sopValue(sop), sendFunction(sendFunc) {}

        void Deserialize(const uint8_t *data, size_t size);

    private:
        Packet packet;
        size_t payloadCounter = 0;
        PacketState state = PacketState::SOP_WAITING;
        PacketHandler packetHandler;
        CRCFunction crcCalculation;
        SendFunction sendFunction;
        uint8_t sopValue;

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
}
