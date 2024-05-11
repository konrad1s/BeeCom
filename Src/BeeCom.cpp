#include "BeeCom.h"

namespace beecom
{
    size_t BeeCOM::Receive()
    {
        uint8_t byte;
        size_t receivedCount = 0;

        while (byteReceiveFunction(&byte))
        {
            deserializer.Deserialize(&byte, 1);
            receivedCount++;
        }

        return receivedCount;
    }

    void BeeCOM::Send(uint8_t packetType, const uint8_t *payload, size_t payloadSize)
    {
        Packet packet;
        packet.header.sop = sopValue;
        packet.header.type = packetType;
        packet.header.length = static_cast<uint16_t>(payloadSize);
        packet.payload = const_cast<uint8_t *>(payload);

        /* Send the header */
        uint8_t headerBuffer[4];
        size_t headerSize = serializer.SerializeHeader(packet, headerBuffer);
        byteTransmitFunction(headerBuffer, headerSize);

        /* Send the payload */
        if (payloadSize > 0U)
        {
            byteTransmitFunction(payload, payloadSize);
        }

        /* Send the CRC */
        uint8_t crcBuffer[2];
        size_t crcSize = serializer.SerializeCRC(packet, crcBuffer);
        byteTransmitFunction(crcBuffer, crcSize);
    }
}
