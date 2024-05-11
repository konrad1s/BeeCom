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
        case State::sopWaiting:
            handleSOPWaiting(byte);
            break;
        case State::typeWaiting:
            handleTypeWaiting(byte);
            break;
        case State::lenLsbWaiting:
            handleLengthLsbWaiting(byte);
            break;
        case State::lenMsbWaiting:
            handleLengthMsbWaiting(byte);
            break;
        case State::gettingPayload:
            handleGettingPayload(byte);
            break;
        case State::crcLsbWaiting:
            handleCRCLowWaiting(byte);
            break;
        case State::crcMsbWaiting:
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
            state = State::typeWaiting;
        }
    }

    void Deserializer::handleTypeWaiting(uint8_t byte)
    {
        packet.header.type = byte;
        state = State::lenLsbWaiting;
    }

    void Deserializer::handleLengthLsbWaiting(uint8_t byte)
    {
        packet.header.length = byte;
        state = State::lenMsbWaiting;
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
            state = State::crcLsbWaiting;
        }
        else
        {
            state = State::gettingPayload;
        }
    }

    void Deserializer::handleCRCLowWaiting(uint8_t byte)
    {
        packet.header.crc = byte;
        state = State::crcMsbWaiting;
    }

    void Deserializer::handleCRCHighWaiting(uint8_t byte)
    {
        packet.header.crc |= static_cast<uint16_t>(byte) << 8;
        processCompletePacket();
    }

    void Deserializer::handleGettingPayload(uint8_t byte)
    {
        if (true == buffer.append(byte))
        {
            if (buffer.getCurrentSize() == packet.header.length)
            {
                state = State::crcLsbWaiting;
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
        if (observer)
        {
            packet.payload = buffer.getBuffer();
            observer->onPacketReceived(packet, crcValid, context);
        }
        resetState();
    }

    bool Deserializer::validateCRC() const
    {
        uint16_t calculatedCRC = crcCalculation(packet.header, buffer.getBuffer(), packet.header.length);
        return calculatedCRC == packet.header.crc;
    }

    void Deserializer::resetState()
    {
        state = State::sopWaiting;
        buffer.clear();
        packet.reset();
    }

}
