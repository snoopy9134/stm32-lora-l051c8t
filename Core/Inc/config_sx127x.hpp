#ifndef CONFIG_SX127X_HPP
#define CONFIG_SX127X_HPP

#include <cstdint>
#include "sx127x_config.hpp"
#include "SX127x.h"

class ConfigWrapper;

constexpr uint8_t REG_OP_MODE       = 0x01;
constexpr uint8_t REG_VERSION       = 0x42;

constexpr uint8_t MODE_SLEEP        = 0x00;
constexpr uint8_t MODE_STDBY        = 0x01;
constexpr uint8_t LONG_RANGE_MODE   = 0x80;


bool init_SX127x(ConfigWrapper& cfg_f);
void sleep_mode(ConfigWrapper& cfg_f);
void standby_mode(ConfigWrapper& cfg_f);

void sendPacket(ConfigWrapper& cfg, const uint8_t* data, uint8_t length);
void sendPacketAM312(ConfigWrapper& cfg, const std::uint8_t* data, uint8_t length);

bool configureLoRa(ConfigWrapper& cfg);


void enterStopMode();

#endif /* CONFIG_SX127X_HPP */