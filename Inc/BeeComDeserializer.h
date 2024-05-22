#pragma once

#include "BeeComTypes.h"
#include "BeeComPacketObserver.h"
#include "BeeComBuffer.h"
#include <cstring>

namespace beecom {
class Deserializer
{
  public:
    enum class State
    {
        sopWaiting,
        typeWaiting,
        lenLsbWaiting,
        lenMsbWaiting,
        gettingPayload,
        crcLsbWaiting,
        crcMsbWaiting,
        packetReceived
    };

    Deserializer(BeeComBuffer& buffer) : buffer(buffer), observer(nullptr), context(nullptr) {}

    void Deserialize(const uint8_t* data, size_t size);
    void SetObserver(IPacketObserver* obs, void* context)
    {
        observer = obs;
        this->context = context;
    }

  private:
    BeeComBuffer& buffer;
    Packet packet;
    State state = State::sopWaiting;
    IPacketObserver* observer;
    void* context;

    void HandleStateChange(uint8_t byte);
    void HandleSOPWaiting(uint8_t byte);
    void HandleTypeWaiting(uint8_t byte);
    void HandleLengthLsbWaiting(uint8_t byte);
    void HandleLengthMsbWaiting(uint8_t byte);
    void HandleCRCLowWaiting(uint8_t byte);
    void HandleCRCHighWaiting(uint8_t byte);
    void HandleGettingPayload(uint8_t byte);
    void ProcessCompletePacket();
    bool ValidateCRC() const;
    void ResetState();
};
} // namespace beecom
