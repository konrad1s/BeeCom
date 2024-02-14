#include "BeeCOM.h"
#include <cstring>

namespace beecom
{
    CRC16AUGCCITTStrategy Receiver::defaultCRCStrategy;

    Receiver::Receiver(PacketHandler callback, ICRCStrategy *crcStrategy)
        : packetHandler(callback), crcStrategy(crcStrategy ? crcStrategy : &defaultCRCStrategy) {}

    void Receiver::Deserialize(const uint8_t *data, size_t size)
    {
        while (size--)
        {
            uint8_t byte = *data++;
            switch (state)
            {
            case PacketState::EMPTY:
                handleStateEmpty(byte);
                break;
            case PacketState::SOP_RECEIVED:
                handleStateGotSOP(byte);
                break;
            case PacketState::TYPE_RECEIVED:
                handleStateGotType(byte);
                break;
            case PacketState::LEN_RECEIVED:
                handleStateGotLength(byte);
                break;
            case PacketState::CRC_LSB_RECEIVED:
                handleStateGotCRCLo(byte);
                break;
            case PacketState::CRC_MSB_RECEIVED:
                handleStateGotCRCHi(byte);
                break;
            case PacketState::GETTING_PAYLOAD:
                handleStateGettingPayload(byte);
                break;
            case PacketState::WHOLE_PACKET_RECEIVED:
                processCompletePacket();
                break;
            default:
                // Handle unexpected state
                resetState();
                break;
            }
        }
    }

    void Receiver::handleStateEmpty(uint8_t byte)
    {
        if (byte == SOP_VALUE)
        {
            packet.header.sop = SOP_VALUE;
            state = PacketState::SOP_RECEIVED;
        }
    }

    void Receiver::handleStateGotSOP(uint8_t byte)
    {
        packet.header.type = byte;
        state = PacketState::TYPE_RECEIVED;
    }

    void Receiver::handleStateGotType(uint8_t byte)
    {
        if (byte <= MAX_PAYLOAD_SIZE)
        {
            packet.header.length = byte;
            state = PacketState::LEN_RECEIVED;
        }
        else
        {
            // Invalid packet length; reset state
            resetState();
        }
    }

    void Receiver::handleStateGotLength(uint8_t byte)
    {
        packet.header.crc = byte;
        state = PacketState::CRC_LSB_RECEIVED;
    }

    void Receiver::handleStateGotCRCLo(uint8_t byte)
    {
        packet.header.crc |= static_cast<uint16_t>(byte) << 8;
        state = PacketState::CRC_MSB_RECEIVED;
    }

    void Receiver::handleStateGotCRCHi(uint8_t byte)
    {
        if (packet.header.length == 0)
        {
            state = PacketState::WHOLE_PACKET_RECEIVED;
        }
        else
        {
            payloadCounter = 0;
            state = PacketState::GETTING_PAYLOAD;
        }
    }

    void Receiver::handleStateGettingPayload(uint8_t byte)
    {
        if (payloadCounter < MAX_PAYLOAD_SIZE)
        {
            packet.payload[payloadCounter++] = byte;
            if (payloadCounter == packet.header.length)
            {
                state = PacketState::WHOLE_PACKET_RECEIVED;
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
        // This function should calculate the CRC of the received packet and compare it with the received CRC.
        // Placeholder for actual CRC calculation and validation
        return true; // Always returns true for this example
    }

    void Receiver::resetState()
    {
        state = PacketState::EMPTY;
        payloadCounter = 0;
        memset(&packet, 0, sizeof(packet)); // Clear the received packet
    }

} // namespace beecom
