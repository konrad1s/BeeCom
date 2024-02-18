#include "BeeCOM.h"
#include <cstring>

namespace beecom
{
    static constexpr uint16_t lookupTable[256] = {CRC16AUGCCITT_LOOKUP};

    uint16_t updateCRC(uint16_t currentCRC, uint8_t dataByte)
    {
        return (currentCRC << 8) ^ lookupTable[((currentCRC >> 8) ^ dataByte) & 0xFF];
    }

    uint16_t calculateFullPacketCRC(const PacketHeader &header, const uint8_t *payload, size_t payloadLength)
    {
        static constexpr uint16_t initialCRCValue = 0x1D0F;
        uint16_t crc = initialCRCValue;

        crc = updateCRC(crc, header.sop);
        crc = updateCRC(crc, header.type);
        crc = updateCRC(crc, header.length);

        for (size_t i = 0; i < payloadLength; ++i)
        {
            crc = updateCRC(crc, payload[i]);
        }

        return crc;
    }

    void Packet::reset()
    {
        *this = Packet();
    }

    size_t Transmitter::Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const
    {
        const size_t requiredSize = calculateRequiredSize(packet);

        if (buffer == nullptr || bufferSize < requiredSize)
        {
            return 0; // Buffer too small or nullptr.
        }

        size_t size = 0;

        buffer[size++] = packet.header.sop;
        buffer[size++] = packet.header.type;
        buffer[size++] = packet.header.length;

        for (size_t i = 0; i < packet.header.length; ++i)
        {
            buffer[size++] = packet.payload[i];
        }

        uint16_t crc = crcCalculation(packet.header, packet.payload, packet.header.length);
        buffer[size++] = static_cast<uint8_t>(crc & 0xFF);
        buffer[size++] = static_cast<uint8_t>((crc >> 8) & 0xFF);

        return size;
    }

    size_t Transmitter::calculateRequiredSize(const Packet &packet) const
    {
        return sizeof(packet.header.sop) + sizeof(packet.header.type) +
               sizeof(packet.header.length) + packet.header.length + sizeof(packet.header.crc);
    }

    Receiver::Receiver(PacketHandler callback, CRCFunction crcFunc)
        : packetHandler(callback), crcCalculation(crcFunc) {}

    void Receiver::Deserialize(const uint8_t *data, size_t size)
    {
        while (size--)
        {
            uint8_t byte = *data++;
            handleStateChange(byte);
        }
    }

    void Receiver::handleStateChange(uint8_t byte)
    {
        switch (state)
        {
        case PacketState::SOP_WAITING:
            handleSOPWaiting(byte);
            break;
        case PacketState::TYPE_WAITING:
            handleTypeWaiting(byte);
            break;
        case PacketState::LEN_WAITING:
            handleLengthWaiting(byte);
            break;
        case PacketState::GETTING_PAYLOAD:
            handleGettingPayload(byte);
            break;
        case PacketState::CRC_LSB_WAITING:
            handleCRCLowWaiting(byte);
            break;
        case PacketState::CRC_MSB_WAITING:
            handleCRCHighWaiting(byte);
            break;
        default:
            resetState();
            break;
        }
    }

    void Receiver::handleSOPWaiting(uint8_t byte)
    {
        if (byte == SOP_VALUE)
        {
            packet.header.sop = SOP_VALUE;
            state = PacketState::TYPE_WAITING;
        }
    }

    void Receiver::handleTypeWaiting(uint8_t byte)
    {
        packet.header.type = byte;
        state = PacketState::LEN_WAITING;
    }

    void Receiver::handleLengthWaiting(uint8_t byte)
    {
        if (byte <= MAX_PAYLOAD_SIZE)
        {
            packet.header.length = byte;
            if (packet.header.length == 0)
            {
                state = PacketState::CRC_LSB_WAITING;
            }
            else
            {
                state = PacketState::GETTING_PAYLOAD;
            }
        }
        else
        {
            resetState(); // Invalid packet length; reset state
        }
    }

    void Receiver::handleCRCLowWaiting(uint8_t byte)
    {
        packet.header.crc = byte;
        state = PacketState::CRC_MSB_WAITING;
    }

    void Receiver::handleCRCHighWaiting(uint8_t byte)
    {
        packet.header.crc |= static_cast<uint16_t>(byte) << 8;
        processCompletePacket();
    }

    void Receiver::handleGettingPayload(uint8_t byte)
    {
        if (payloadCounter < MAX_PAYLOAD_SIZE)
        {
            packet.payload[payloadCounter++] = byte;
            if (payloadCounter == packet.header.length)
            {
                state = PacketState::CRC_LSB_WAITING;
            }
        }
        else
        {
            resetState(); // Payload overflow; reset state
        }
    }

    void Receiver::processCompletePacket()
    {
        if (validateCRC())
        {
            packetHandler(packet, true);
        }
        else
        {
            packetHandler(packet, false);
        }
        resetState();
    }

    bool Receiver::validateCRC() const
    {
        uint16_t calculatedCRC = crcCalculation(packet.header, packet.payload, packet.header.length);
        uint16_t receivedCRC = (static_cast<uint16_t>(packet.header.crc) << 8) | static_cast<uint16_t>(packet.header.crc >> 8);

        return calculatedCRC == receivedCRC;
    }

    void Receiver::resetState()
    {
        state = PacketState::SOP_WAITING;
        payloadCounter = 0;
        std::memset(&packet, 0, sizeof(packet));
    }

    void BeeCOM::receive()
    {
        uint8_t byte;

        while (byteReceiveFunction(&byte))
        {
            receiver.Deserialize(&byte, 1);
        }
    }

    void BeeCOM::send(const Packet &packet)
    {
        uint8_t buffer[MAX_PAYLOAD_SIZE + sizeof(PacketHeader)];
        size_t size = transmitter.Serialize(packet, buffer, sizeof(buffer));

        if (size > 0)
        {
            byteTransmitFunction(buffer, size);
        }
    }

    size_t BeeCOM::Serialize(const Packet &packet, uint8_t *buffer, size_t bufferSize) const
    {
        return transmitter.Serialize(packet, buffer, bufferSize);
    }

} // namespace beecom
