#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#include <cstdint>

#include "main.hpp"
#include "sx127x_config.hpp"
#include "SX127x.h"

#define SLAVE_DEVICE_STM32L051C8Tx
// #define SLAVE_DEVICE_STM32F446RE

class ConfigWrapper
{
  public:
	static ConfigWrapper& GetInstance();

	ConfigWrapper& operator=(ConfigWrapper&&) = delete;
	ConfigWrapper& operator=(const ConfigWrapper&) = delete;

	ConfigWrapper (ConfigWrapper&&) = delete;
	ConfigWrapper (const ConfigWrapper&) = delete;

	~ConfigWrapper() = default;

	void csLow();
	void csHigh();

	void init(SPI_HandleTypeDef* spi);

	void resetLow();
	void resetHigh();

	void delayMs(std::uint32_t ms);

	std::uint8_t spiTransfer(std::uint8_t data);

	uint8_t readRegister(uint8_t addr);
	void writeRegister(uint8_t addr, uint8_t value);

  private:
    ConfigWrapper() = default;

    SPI_HandleTypeDef* spiHandle = nullptr;

};

#endif // CONFIG_WRAPPER_HPP
