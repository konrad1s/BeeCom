#include <cstring>
#include "BeeComDeserializer.h"

namespace beecom
{
    void Deserializer::Deserialize(const uint8_t *data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            HandleStateChange(data[i]);
        }
    }

    void Deserializer::HandleStateChange(uint8_t byte)
    {
        switch (state)
        {
        case State::sopWaiting:
            HandleSOPWaiting(byte);
            break;
        case State::typeWaiting:
            HandleTypeWaiting(byte);
            break;
        case State::lenLsbWaiting:
            HandleLengthLsbWaiting(byte);
            break;
        case State::lenMsbWaiting:
            HandleLengthMsbWaiting(byte);
            break;
        case State::gettingPayload:
            HandleGettingPayload(byte);
            break;
        case State::crcLsbWaiting:
            HandleCRCLowWaiting(byte);
            break;
        case State::crcMsbWaiting:
            HandleCRCHighWaiting(byte);
            break;
        default:
            ResetState();
            break;
        }
    }

    void Deserializer::HandleSOPWaiting(uint8_t byte)
    {
        if (byte == sopValue)
        {
            packet.header.sop = byte;
            state = State::typeWaiting;
        }
    }

    void Deserializer::HandleTypeWaiting(uint8_t byte)
    {
        packet.header.type = byte;
        state = State::lenLsbWaiting;
    }

    void Deserializer::HandleLengthLsbWaiting(uint8_t byte)
    {
        packet.header.length = byte;
        state = State::lenMsbWaiting;
    }

    void Deserializer::HandleLengthMsbWaiting(uint8_t byte)
    {
        packet.header.length |= static_cast<uint16_t>(byte) << 8;
        if (packet.header.length > MAX_PAYLOAD_SIZE)
        {
            ResetState();
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

    void Deserializer::HandleCRCLowWaiting(uint8_t byte)
    {
        packet.header.crc = byte;
        state = State::crcMsbWaiting;
    }

    void Deserializer::HandleCRCHighWaiting(uint8_t byte)
    {
        packet.header.crc |= static_cast<uint16_t>(byte) << 8;
        ProcessCompletePacket();
    }

    void Deserializer::HandleGettingPayload(uint8_t byte)
    {
        if (true == buffer.Append(byte))
        {
            if (buffer.GetCurrentSize() == packet.header.length)
            {
                state = State::crcLsbWaiting;
            }
        }
        else
        {
            ResetState();
        }
    }

    void Deserializer::ProcessCompletePacket()
    {
        bool crcValid = ValidateCRC();
        if (observer)
        {
            packet.payload = buffer.GetBuffer();
            observer->OnPacketReceived(packet, crcValid, context);
        }
        ResetState();
    }

    bool Deserializer::ValidateCRC() const
    {
        uint16_t calculatedCRC = crcCalculation(packet.header, buffer.GetBuffer(), packet.header.length);
        return calculatedCRC == packet.header.crc;
    }

    void Deserializer::ResetState()
    {
        state = State::sopWaiting;
        buffer.Clear();
        packet.Reset();
    }

}
