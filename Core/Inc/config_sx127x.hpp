#ifndef CONFIG_SX127X_HPP
#define CONFIG_SX127X_HPP

#include <cstdint>
#include "sx127x_config.hpp"
#include "SX127x.h"

class ConfigWrapper;

 // constexpr uint8_t REG_OP_MODE       = 0x01;
// constexpr uint8_t REG_VERSION       = 0x42;

// constexpr uint8_t MODE_SLEEP        = 0x00;
// constexpr uint8_t MODE_STDBY        = 0x01;
// constexpr uint8_t LONG_RANGE_MODE   = 0x80;

 // constexpr uint8_t REG_IRQ_FLAGS = 0x12;
constexpr uint8_t REG_FIFO      = 0x00;

// constexpr uint8_t IRQ_RX_DONE   = 0x40;
constexpr uint8_t IRQ_TX_DONE   = 0x08;
// constexpr uint8_t IRQ_CRC_ERROR = 0x20;

 // constexpr uint8_t MODE_TX            = 0x03;
 // constexpr uint8_t MODE_RX_CONTINUOUS = 0x05;
constexpr uint8_t MODE_RX_SINGLE     = 0x06;

bool init_SX127x(ConfigWrapper& cfg_f);
void sleep_mode(ConfigWrapper& cfg_f);
void standby_mode(ConfigWrapper& cfg_f);

void sendPacket(ConfigWrapper& cfg, const uint8_t* data, uint8_t length);
void sendPacketAM312(ConfigWrapper& cfg, const std::uint8_t* data, uint8_t length);

bool configureLoRa(ConfigWrapper& cfg);


void enterStopMode();

bool waitForAck(ConfigWrapper& cfg,
                uint32_t timeoutMs,
                const uint8_t expectedCounter);

bool verifyAckData(ConfigWrapper& cfg, const uint8_t expectedCounter);

#endif /* CONFIG_SX127X_HPP */