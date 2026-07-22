#include "protocol.hpp"

DataProtocol::DataProtocol( std::uint8_t version_mj, std::uint8_t version_mn, std::uint8_t id) 
                : version_major(version_mj), 
                version_minor(version_mn),
                m_deviceId(id), 
                m_counter(0), 
                m_voltage(0),
                m_crc(0), 
                m_messageType(MessageType::MESSAGE) {}