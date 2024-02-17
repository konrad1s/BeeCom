#include "BeeCOM.h"
#include <cstring>

namespace beecom
{
    CRC16AUGCCITTStrategy Receiver::defaultCRCStrategy;

    void Receiver::Deserialize(const uint8_t *data, size_t size)
    {
        while (size--)
        {
            uint8_t byte = *data++;
            switch (state)
            {
            case PacketState::SOP_WAITING:
                handleWaitingForSOP(byte);
                break;
            case PacketState::TYPE_WAITING:
                handleWaitingForType(byte);
                break;
            case PacketState::LEN_WAITING:
                handleWaitingForLength(byte);
                break;
            case PacketState::CRC_LSB_WAITING:
                handleWaitingForCRCLow(byte);
                break;
            case PacketState::CRC_MSB_WAITING:
                handleWaitingForCRCHigh(byte);
                break;
            case PacketState::GETTING_PAYLOAD:
                handleGettingPayload(byte);
                break;
            default:
                resetState();
                break;
            }
        }
    }

    void Receiver::handleWaitingForSOP(uint8_t byte)
    {
        if (byte == SOP_VALUE)
        {
            packet.header.sop = SOP_VALUE;
            state = PacketState::TYPE_WAITING;
        }
    }

    void Receiver::handleWaitingForType(uint8_t byte)
    {
        packet.header.type = byte;
        state = PacketState::LEN_WAITING;
    }

    void Receiver::handleWaitingForLength(uint8_t byte)
    {
        if (byte <= MAX_PAYLOAD_SIZE)
        {
            packet.header.length = byte;
            state = PacketState::CRC_LSB_WAITING;
        }
        else
        {
            // Invalid packet length; reset state
            resetState();
        }
    }

    void Receiver::handleWaitingForCRCLow(uint8_t byte)
    {
        packet.header.crc = byte;
        state = PacketState::CRC_MSB_WAITING;
    }

    void Receiver::handleWaitingForCRCHigh(uint8_t byte)
    {
        packet.header.crc |= static_cast<uint16_t>(byte) << 8;
        if (packet.header.length == 0)
        {
            processCompletePacket();
        }
        else
        {
            payloadCounter = 0;
            state = PacketState::GETTING_PAYLOAD;
        }
    }

    void Receiver::handleGettingPayload(uint8_t byte)
    {
        if (payloadCounter < MAX_PAYLOAD_SIZE)
        {
            packet.payload[payloadCounter++] = byte;
            if (payloadCounter == packet.header.length)
            {
                processCompletePacket();
            }
        }
        else
        {
            // Payload overflow; reset state
            resetState();
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
        uint16_t calculatedCRC = crcStrategy->calculate(packet.payload, packet.header.length);

        uint16_t receivedCRC = (static_cast<uint16_t>(packet.header.crc) << 8) | static_cast<uint16_t>(packet.header.crc >> 8);

        return calculatedCRC == receivedCRC;
    }

    void Receiver::resetState()
    {
        state = PacketState::SOP_WAITING;
        payloadCounter = 0;
        memset(&packet, 0, sizeof(packet)); // Clear the received packet
    }

} // namespace beecom
