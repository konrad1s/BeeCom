#include "BeeCOM.h"
#include <cstring>

namespace beecom
{
    CRC16AUGCCITTStrategy Receiver::defaultCRCStrategy;

    uint16_t CRC16AUGCCITTStrategy::calculateFullPacketCRC(const PacketHeader &header, const uint8_t *payload, size_t payloadLength) const
    {
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

    uint16_t CRC16AUGCCITTStrategy::updateCRC(uint16_t currentCRC, uint8_t dataByte) const
    {
        return (currentCRC << 8) ^ lookupTable[((currentCRC >> 8) ^ dataByte) & 0xFF];
    }

    void Packet::reset()
    {
        *this = Packet();
    }

    Receiver::Receiver(PacketHandler callback, ICRCStrategy *crcStrategy)
        : packetHandler(callback), crcStrategy(crcStrategy ? crcStrategy : &defaultCRCStrategy) {}

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
        uint16_t calculatedCRC = crcStrategy->calculateFullPacketCRC(packet.header, packet.payload, packet.header.length);
        uint16_t receivedCRC = (static_cast<uint16_t>(packet.header.crc) << 8) | static_cast<uint16_t>(packet.header.crc >> 8);

        return calculatedCRC == receivedCRC;
    }

    void Receiver::resetState()
    {
        state = PacketState::SOP_WAITING;
        payloadCounter = 0;
        std::memset(&packet, 0, sizeof(packet));
    }

} // namespace beecom
