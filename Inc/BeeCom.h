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

        BeeCOM(
            ByteReceiveFunction byteReceiver,
            ByteTransmitFunction byteTransmitter,
            CRCFunction crcFunc = calculateFullPacketCRC,
            uint8_t receiverSop = 0xA5U,
            InvalidPacketHandler invalidHandler = [](SendFunction) {})
            : transmitter(crcFunc),
              byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter),
              receiver(
                  crcFunc, receiverSop,
                  [this](const Packet &packet)
                  { this->send(packet); },
                  invalidHandler)
        {
        }

        void receive();
        void send(const Packet &packet);
        void setPacketHandler(PacketHandler handler) 
        {
            receiver.setPacketHandler(handler);
        }

    private:
        Receiver receiver;
        Transmitter transmitter;
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
    };
}
