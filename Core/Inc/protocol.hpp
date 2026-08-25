#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstdint>

//Software specific
constexpr std::uint8_t VERSION_MAJOR = 0x01;
constexpr std::uint8_t VERSION_MINOR = 0x00;

enum class MessageType : std::uint8_t
{
    MESSAGE,
    RESPONSE
};

// Data protocol to be sent from slave to master
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
        void setVoltage(std::uint16_t voltage) { m_voltage = voltage; }

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
            buffer[4] = static_cast<uint8_t>((m_voltage & 0xFF00)>>8);
            buffer[5] = static_cast<uint8_t>(m_voltage & 0x00FF);
            buffer[6] = m_crc;
            buffer[7] = static_cast<uint8_t>(m_messageType);
        }

    private:
        std::uint8_t version_major;
        std::uint8_t version_minor;
        std::uint8_t m_deviceId;
        std::uint8_t m_counter;
        std::uint16_t m_voltage;
        std::uint8_t m_crc;
        MessageType m_messageType;
        // in case additional data introduced to protocol, 
        // uint8_t dataBuffer[8] has to get updated as well
};

// Data protocol to be received from master by slave
struct RxDataProtocol
{
    std::uint8_t version_major;
    std::uint8_t version_minor;
    std::uint8_t m_slaveDeviceId;
    std::uint8_t m_masterDeviceId;
    std::uint8_t m_counter;
    std::uint8_t m_crc;
    MessageType m_messageType;
};

#endif /* PROTOCOL_HPP */