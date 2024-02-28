#pragma once

#include <cstdint>
#include <functional>
#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include "BeeComTransmitter.h"
#include "BeeComReceiver.h"

namespace beecom
{
    class BeeCOM
    {
    public:
        using ByteReceiveFunction = std::function<bool(uint8_t *byte)>;
        using ByteTransmitFunction = std::function<void(const uint8_t *buffer, size_t size)>;

        BeeCOM(PacketHandler packetHandler,
               ByteReceiveFunction byteReceiver,
               ByteTransmitFunction byteTransmitter,
               CRCFunction crcFunc = calculateFullPacketCRC,
               uint8_t receiverSop = 0xA5U)
            : receiver(packetHandler, crcFunc, receiverSop, [this](const Packet& packet) { this->send(packet); }),
              transmitter(crcFunc),
              byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter)
        {
        }

        void receive();
        void send(const Packet &packet);
        size_t Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const;

    private:
        Receiver receiver;
        Transmitter transmitter;
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
    };
}
