#pragma once
#include "BeeComTypes.h"

namespace beecom
{
    class IPacketObserver
    {
    public:
        virtual ~IPacketObserver() = default;
        virtual void OnPacketReceived(const Packet &packet, bool crcValid, void *beeComInstance) = 0;
    };
}
