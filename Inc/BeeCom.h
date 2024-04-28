#pragma once

#include <cstdint>
#include <functional>
#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include "BeeComSerializer.h"
#include "BeeComDeserializer.h"

namespace beecom
{
    class BeeCOM
    {
    public:
        using ByteReceiveFunction = std::function<bool(uint8_t *byte)>;
        using ByteTransmitFunction = std::function<void(const uint8_t *buffer, size_t size)>;

        BeeCOM(
            ByteReceiveFunction byteReceiver,
            ByteTransmitFunction byteTransmitter,
            CRCFunction crcFunc = calculateFullPacketCRC,
            uint8_t sopValue = 0xA5)
            : serializer(crcFunc),
              byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter),
              deserializer(crcFunc, sopValue),
              sopValue(sopValue)
        {
        }

        size_t receive();
        void send(uint8_t packetType, const uint8_t *payload, size_t payloadSize);
        void setPacketHandler(PacketHandler handler) 
        {
            deserializer.setPacketHandler(handler);
        }

    private:
        Deserializer deserializer;
        Serializer serializer;
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
        uint8_t sopValue;
    };
}
