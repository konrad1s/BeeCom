#include "BeeCom.h"

namespace beecom
{
    size_t BeeCOM::receive()
    {
        uint8_t byte;
        size_t receivedCount = 0;

        while (byteReceiveFunction(&byte))
        {
            receiver.Deserialize(&byte, 1);
            receivedCount++;
        }

        return receivedCount;
    }

    void BeeCOM::send(const Packet& packet)
    {
        uint8_t buffer[MAX_PAYLOAD_SIZE + sizeof(PacketHeader)];
        size_t size = transmitter.Serialize(packet, buffer, sizeof(buffer));

        if (size > 0U)
        {
            byteTransmitFunction(buffer, size);
        }
    }
}
