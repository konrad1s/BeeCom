#include "BeeCOM.h"

namespace beecom
{
    void BeeCOM::receive()
    {
        uint8_t byte;

        while (byteReceiveFunction(&byte))
        {
            receiver.Deserialize(&byte, 1);
        }
    }

    void BeeCOM::send(const Packet& packet)
    {
        uint8_t buffer[MAX_PAYLOAD_SIZE + sizeof(PacketHeader)];
        size_t size = Serialize(packet, buffer, sizeof(buffer));

        if (size > 0U)
        {
            byteTransmitFunction(buffer, size);
        }
    }

    size_t BeeCOM::Serialize(const Packet& packet, uint8_t* buffer, size_t bufferSize) const
    {
        return transmitter.Serialize(packet, buffer, bufferSize);
    }
}
