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
            uint8_t receiverSop = 0xA5)
            : serializer(crcFunc),
              byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter),
              deserializer(
                  crcFunc, receiverSop,
                  [this](const Packet &packet)
                  { this->send(packet); })
        {
        }

        size_t receive();
        void send(const Packet &packet);
        void setPacketHandler(PacketHandler handler) 
        {
            deserializer.setPacketHandler(handler);
        }

    private:
        Deserializer deserializer;
        Serializer serializer;
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
    };
}
