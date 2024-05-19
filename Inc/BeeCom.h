#pragma once

#include <cstdint>
#include "BeeComTypes.h"
#include "BeeComCrc.h"
#include "BeeComSerializer.h"
#include "BeeComDeserializer.h"
#include "BeeComPacketObserver.h"
#include "BeeComBuffer.h"

namespace beecom {
class BeeCOM
{
  public:
    using ByteReceiveFunction = bool (*)(uint8_t* byte);
    using ByteTransmitFunction = void (*)(const uint8_t* buffer, size_t size);

    BeeCOM(
        ByteReceiveFunction byteReceiver,
        ByteTransmitFunction byteTransmitter,
        BeeComBuffer& buffer) :
        byteReceiveFunction(byteReceiver),
        byteTransmitFunction(byteTransmitter),
        serializer(),
        deserializer(buffer)
    {
    }

    size_t Receive();
    void Send(uint8_t packetType, const uint8_t* payload, size_t payloadSize);
    void SetObserver(IPacketObserver* handler)
    {
        deserializer.SetObserver(handler, this);
    }

  private:
    ByteReceiveFunction byteReceiveFunction;
    ByteTransmitFunction byteTransmitFunction;
    Serializer serializer;
    Deserializer deserializer;
};
} // namespace beecom
