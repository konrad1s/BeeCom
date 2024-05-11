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
            CRCFunction crcFunc = calculateFullPacketCRC,
            uint8_t sopValue = 0xA5U)
            : serializer(crcFunc),
              byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter),
              deserializer(buffer, crcFunc, sopValue),
              sopValue(sopValue)
        {
        }

        size_t receive();
        void send(uint8_t packetType, const uint8_t *payload, size_t payloadSize);
        void setObserver(IPacketObserver *handler)
        {
            deserializer.setObserver(handler, this);
        }

    private:
        Deserializer deserializer;
        Serializer serializer;
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
        uint8_t sopValue;
    };
}
