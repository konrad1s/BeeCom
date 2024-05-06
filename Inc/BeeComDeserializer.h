#pragma once

#include "BeeComTypes.h"
#include "BeeComPacketObserver.h"
#include <cstring>

namespace beecom
{
    class Deserializer
    {
    public:
        enum class State
        {
            sopWaiting,
            typeWaiting,
            lenLsbWaiting,
            lenMsbWaiting,
            gettingPayload,
            crcLsbWaiting,
            crcMsbWaiting,
            packetReceived
        };

        Deserializer(CRCFunction crcFunc, uint8_t sop)
            : crcCalculation(crcFunc), sopValue(sop), observer(nullptr)
        {
        }

        void Deserialize(const uint8_t *data, size_t size);
        void setObserver(IPacketObserver *obs, void *context)
        {
            observer = obs;
            this->context = context;
        }

    private:
        Packet packet;
        size_t payloadCounter = 0;
        State state = State::sopWaiting;
        CRCFunction crcCalculation;
        uint8_t sopValue;
        IPacketObserver* observer;
        void *context;

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
