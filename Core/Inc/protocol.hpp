#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstdint>

enum class MessageType : std::uint8_t
{
    MESSAGE,
    RESPONSE
};

class DataProtocol
{
    public:
        DataProtocol(std::uint8_t version_mj, std::uint8_t version_mn, std::uint8_t id);

        DataProtocol (DataProtocol&&) = delete;
        DataProtocol (const DataProtocol&) = delete;

        DataProtocol& operator=(DataProtocol &&) = delete;
        DataProtocol& operator=(const DataProtocol&) = delete;

        void setCounter(std::uint8_t counter) { m_counter = counter; }
        void setCrc(std::uint8_t crc) { m_crc = crc; }
        void setMessageType(MessageType type) { m_messageType = type; }
        const uint8_t* data() const
        {
            return reinterpret_cast<const uint8_t*>(this);
        }

        uint8_t size() const
        {
            return sizeof(DataProtocol);
        }

        void serialize(uint8_t* buffer) const
        {
            buffer[0] = version_major;
            buffer[1] = version_minor;
            buffer[2] = m_deviceId;
            buffer[3] = m_counter;
            buffer[4] = m_crc;
            buffer[5] = static_cast<uint8_t>(m_messageType);
        }

    private:
        std::uint8_t version_major;
        std::uint8_t version_minor;
        std::uint8_t m_deviceId;
        std::uint8_t m_counter;
        std::uint8_t m_crc;
        MessageType m_messageType;
        // std::uint8_t m_buffer[6];
};

#endif /* PROTOCOL_HPP */