#include <cstring>
#include "BeeComDeserializer.h"

namespace beecom
{
    void Deserializer::Deserialize(const uint8_t *data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            handleStateChange(data[i]);
        }
    }

    void Deserializer::handleStateChange(uint8_t byte)
    {
        switch (state)
        {
        case PacketState::SOP_WAITING:
            handleSOPWaiting(byte);
            break;
        case PacketState::TYPE_WAITING:
            handleTypeWaiting(byte);
            break;
        case PacketState::LEN_LSB_WAITING:
            handleLengthLsbWaiting(byte);
            break;
        case PacketState::LEN_MSB_WAITING:
            handleLengthMsbWaiting(byte);
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

    void Deserializer::handleSOPWaiting(uint8_t byte)
    {
        if (byte == sopValue)
        {
            packet.header.sop = byte;
            state = PacketState::TYPE_WAITING;
        }
    }

    void Deserializer::handleTypeWaiting(uint8_t byte)
    {
        packet.header.type = byte;
        state = PacketState::LEN_LSB_WAITING;
    }

    void Deserializer::handleLengthLsbWaiting(uint8_t byte)
    {
        packet.header.length = byte;
        state = PacketState::LEN_MSB_WAITING;
    }

    void Deserializer::handleLengthMsbWaiting(uint8_t byte)
    {
        packet.header.length |= static_cast<uint16_t>(byte) << 8;
        if (packet.header.length > MAX_PAYLOAD_SIZE)
        {
            resetState();
        }
        else if (packet.header.length == 0)
        {
            state = PacketState::CRC_LSB_WAITING;
        }
        else
        {
            state = PacketState::GETTING_PAYLOAD;
        }
    }

    void Deserializer::handleCRCLowWaiting(uint8_t byte)
    {
        packet.header.crc = byte;
        state = PacketState::CRC_MSB_WAITING;
    }

    void Deserializer::handleCRCHighWaiting(uint8_t byte)
    {
        packet.header.crc |= static_cast<uint16_t>(byte) << 8;
        processCompletePacket();
    }

    void Deserializer::handleGettingPayload(uint8_t byte)
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
            resetState();
        }
    }

    void Deserializer::processCompletePacket()
    {
        bool crcValid = validateCRC();
        if (packetHandler)
        {
            packetHandler(packet, crcValid, sendFunction);
        }
        resetState();
    }

    bool Deserializer::validateCRC() const
    {
        uint16_t calculatedCRC = crcCalculation(packet.header, packet.payload, packet.header.length);
        return calculatedCRC == packet.header.crc;
    }

    void Deserializer::resetState()
    {
        state = PacketState::SOP_WAITING;
        payloadCounter = 0;
        packet.reset();
    }

}
