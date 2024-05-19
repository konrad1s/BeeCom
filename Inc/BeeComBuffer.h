#pragma once

#include <cstdint>
#include <cstring>

namespace beecom {
class BeeComBuffer
{
  public:
    BeeComBuffer(uint8_t* buffer, size_t size) : buffer(buffer), bufferSize(size), currentSize(0) {}

    uint8_t* GetBuffer() const;
    size_t GetCurrentSize() const;
    size_t GetBufferSize() const;
    void Clear();
    bool Append(const uint8_t* data, size_t size);
    bool Append(uint8_t byte);

  private:
    uint8_t* buffer;
    size_t bufferSize;
    size_t currentSize;
};
} // namespace beecom
