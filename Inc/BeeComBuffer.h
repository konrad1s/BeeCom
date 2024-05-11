#pragma once

#include <cstdint>
#include <cstring>

namespace beecom
{
    class BeeComBuffer
    {
    public:
        BeeComBuffer(uint8_t *buffer, size_t size)
            : buffer(buffer), bufferSize(size), currentSize(0)
        {
        }

        uint8_t *getBuffer() const
        {
            return const_cast<uint8_t *>(buffer);
        }

        size_t getCurrentSize() const
        {
            return currentSize;
        }

        void clear()
        {
            currentSize = 0U;
        }

        bool append(const uint8_t *data, size_t size)
        {
            if (currentSize + size > bufferSize)
            {
                return false;
            }

            memcpy(buffer + currentSize, data, size);
            currentSize += size;

            return true;
        }

        bool append(uint8_t byte)
        {
            if (currentSize + 1U > bufferSize)
            {
                return false;
            }

            buffer[currentSize] = byte;
            currentSize++;

            return true;
        }

    private:
        uint8_t *buffer;
        size_t bufferSize;
        size_t currentSize;
    };
} // namespace beecom
