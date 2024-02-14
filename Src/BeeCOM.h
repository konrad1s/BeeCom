#ifndef BEECOM_H_
#define BEECOM_H_

#include <cstdint>
#include <cassert>

namespace beecom
{
    class ICRCStrategy
    {
    public:
        virtual ~ICRCStrategy() = default;
        virtual uint16_t calculate(const uint8_t *data, size_t length) const = 0;
        virtual uint16_t updateCRC(uint16_t currentCRC, uint8_t dataByte) const = 0;
    };

    class CRC16AUGCCITTStrategy : public ICRCStrategy
    {
    public:
        static constexpr uint16_t initialCRCValue = 0x1D0F;

    private:
        static constexpr uint16_t lookupTable[256] = {/* Table values */};

    public:
        uint16_t calculate(const uint8_t *data, size_t length) const override
        {
            uint16_t crc = initialCRCValue;
            for (size_t i = 0; i < length; ++i)
            {
                crc = updateCRC(crc, data[i]);
            }
            return crc;
        }

        uint16_t updateCRC(uint16_t currentCRC, uint8_t dataByte) const override
        {
            return (currentCRC << 8) ^ lookupTable[((currentCRC >> 8) ^ dataByte) & 0xFF];
        }
    };

    constexpr uint8_t MAX_PAYLOAD_SIZE = 200U;
    constexpr uint8_t SOP_VALUE = 0xA5U;

    struct PacketHeader
    {
        uint8_t sop = SOP_VALUE;
        uint8_t type = 0U;
        uint8_t length = 0U;
        uint16_t crc = 0U;
    };

    struct Packet
    {
        PacketHeader header;
        uint8_t payload[MAX_PAYLOAD_SIZE] = {};

        void reset()
        {
            header = PacketHeader();
        }
    };

    enum class PacketState
    {
        EMPTY,
        SOP_RECEIVED,
        TYPE_RECEIVED,
        LEN_RECEIVED,
        CRC_LSB_RECEIVED,
        CRC_MSB_RECEIVED,
        GETTING_PAYLOAD,
        WHOLE_PACKET_RECEIVED
    };

    using PacketHandler = void (*)(const Packet &packet);

    class Receiver
    {
    public:
        Receiver(PacketHandler callback, ICRCStrategy *crcStrategy = nullptr)
            : packetHandler(callback), crcStrategy(crcStrategy ? crcStrategy : &defaultCRCStrategy) {}

        void Deserialize(const uint8_t *data, size_t size);

    private:
        Packet packet;
        size_t payloadCounter = 0;
        PacketState state = PacketState::EMPTY;
        PacketHandler packetHandler;
        ICRCStrategy *crcStrategy;
        static CRC16AUGCCITTStrategy defaultCRCStrategy;

        void handleStateEmpty(uint8_t byte);
        void handleStateGotSOP(uint8_t byte);
        void handleStateGotType(uint8_t byte);
        void handleStateGotLength(uint8_t byte);
        void handleStateGotCRCLo(uint8_t byte);
        void handleStateGotCRCHi(uint8_t byte);
        void handleStateGettingPayload(uint8_t byte);
        void processCompletePacket();
        bool validateCRC() const;
        void resetState();
        void updateHeaderCRC(uint8_t byte);
        void transitionState(PacketState newState);
    };

} // namespace beecom

#endif /* BEECOM_H_ */
