#ifndef CONFIG_SX127X_HPP
#define CONFIG_SX127X_HPP

#include <cstdint>
#include "sx127x_config.hpp"
#include "SX127x.h"

class ConfigWrapper;

constexpr uint8_t REG_FIFO      = 0x00;
constexpr uint8_t IRQ_TX_DONE   = 0x08;
constexpr uint8_t MODE_RX_SINGLE     = 0x06;

/// Initialize the SX127x radio using the given configuration wrapper.
bool init_SX127x(ConfigWrapper& cfg_f);

/// Put the SX127x into low-power sleep mode.
void sleep_mode(ConfigWrapper& cfg_f);

/// Place the SX127x in standby for quick resume.
void standby_mode(ConfigWrapper& cfg_f);

/// Send a normal packet with the provided payload.
void sendPacket(ConfigWrapper& cfg, const uint8_t* data, uint8_t length);

/// Apply LoRa radio settings such as frequency and bandwidth.
bool configureLoRa(ConfigWrapper& cfg);

/// Enter MCU stop mode to reduce power consumption.
void enterStopMode();

/// Wait for an acknowledgement packet until timeout.
bool waitForAck(ConfigWrapper& cfg,
                uint32_t timeoutMs,
                const uint8_t expectedCounter);

/// Verify that the received ACK contains the expected counter.
bool verifyAckData(ConfigWrapper& cfg, const uint8_t expectedCounter);

#endif /* CONFIG_SX127X_HPP */