#pragma once

#include "BeeComTypes.h"
#include <cstring>

namespace beecom {
class Serializer
{
  public:
    explicit Serializer() = default;

    size_t Serialize(const Packet& packet, uint8_t* buffer, size_t bufferSize) const;
    size_t SerializeHeader(const Packet& packet, uint8_t* buffer) const;
    size_t SerializeCRC(const Packet& packet, uint8_t* buffer) const;

  private:
    size_t CalculateRequiredSize(const Packet& packet) const;
};
} // namespace beecom
