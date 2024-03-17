#include <cstring>
#include "BeeComReceiver.h"

namespace beecom
{
    void Receiver::Deserialize(const uint8_t *data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            handleStateChange(data[i]);
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
        if (byte == sopValue)
        {
            packet.header.sop = byte;
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
            resetState();
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
            resetState();
        }
    }

    void Receiver::processCompletePacket()
    {
        bool crcValid = validateCRC();
        if (packetHandler)
        {
            packetHandler(packet, crcValid, sendFunction);
        }
        resetState();
    }

    bool Receiver::validateCRC() const
    {
        uint16_t calculatedCRC = crcCalculation(packet.header, packet.payload, packet.header.length);
        return calculatedCRC == packet.header.crc;
    }

    void Receiver::resetState()
    {
        state = PacketState::SOP_WAITING;
        payloadCounter = 0;
        packet.reset();
        if (invalidPacketHandler)
        {
            invalidPacketHandler(sendFunction);
        }
    }

}
