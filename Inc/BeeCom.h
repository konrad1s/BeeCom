#pragma once

#include <cstdint>
#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include "BeeComSerializer.h"
#include "BeeComDeserializer.h"
#include "BeeComPacketObserver.h"
#include "BeeComBuffer.h"

namespace beecom
{
    class BeeCOM
    {
    public:
        using ByteReceiveFunction = bool (*)(uint8_t* byte);
        using ByteTransmitFunction = void (*)(const uint8_t* buffer, size_t size);

        BeeCOM(
            ByteReceiveFunction byteReceiver,
            ByteTransmitFunction byteTransmitter,
            BeeComBuffer& buffer,
            CRCFunction crcFunc = CalculateFullPacketCRC,
            uint8_t sopValue = 0xA5U)
            : byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter),
              serializer(crcFunc),
              deserializer(buffer, crcFunc, sopValue),
              sopValue(sopValue)
        {
        }

        size_t Receive();
        void Send(uint8_t packetType, const uint8_t *payload, size_t payloadSize);
        void SetObserver(IPacketObserver *handler)
        {
            deserializer.SetObserver(handler, this);
        }

    private:
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
        Serializer serializer;
        Deserializer deserializer;
        uint8_t sopValue;
    };
}
