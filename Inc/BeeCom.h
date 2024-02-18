#ifndef BEECOM_H_
#define BEECOM_H_

#include <cstdint>
#include <cassert>
#include <cstddef>
#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include "BeeComTransmitter.h"
#include "BeeComReceiver.h"

namespace beecom
{
    class BeeCOM
    {
    public:
        using ByteReceiveFunction = bool (*)(uint8_t *byte);
        using ByteTransmitFunction = void (*)(const uint8_t *buffer, size_t size);
        static BeeCOM *instance;

        BeeCOM(PacketHandler packetHandler,
               ByteReceiveFunction byteReceiver,
               ByteTransmitFunction byteTransmitter,
               CRCFunction crcFunc = calculateFullPacketCRC)
            : receiver(packetHandler, crcFunc),
              transmitter(crcFunc),
              byteReceiveFunction(byteReceiver),
              byteTransmitFunction(byteTransmitter)
        {
            instance = this;
            receiver.setSendFunction(StaticSendFunction);
        }

        void receive();
        void send(const Packet &packet);
        size_t Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const;

    private:
        Receiver receiver;
        Transmitter transmitter;
        ByteReceiveFunction byteReceiveFunction;
        ByteTransmitFunction byteTransmitFunction;
        static void StaticSendFunction(const Packet &packet)
        {
            instance->send(packet);
        }
    };

} // namespace beecom

#endif /* BEECOM_H_ */
