#include "BeeComBuffer.h"

namespace beecom {

uint8_t* BeeComBuffer::GetBuffer() const
{
    return const_cast<uint8_t*>(buffer);
}

size_t BeeComBuffer::GetCurrentSize() const
{
    return currentSize;
}

size_t BeeComBuffer::GetBufferSize() const
{
    return bufferSize;
}

void BeeComBuffer::Clear()
{
    currentSize = 0U;
}

bool BeeComBuffer::Append(const uint8_t* data, size_t size)
{
    if (currentSize + size > bufferSize)
    {
        return false;
    }

    memcpy(buffer + currentSize, data, size);
    currentSize += size;

    return true;
}

bool BeeComBuffer::Append(uint8_t byte)
{
    if (currentSize + 1U > bufferSize)
    {
        return false;
    }

    buffer[currentSize] = byte;
    currentSize++;

    return true;
}
} // namespace beecom
