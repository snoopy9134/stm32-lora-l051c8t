#ifndef CONFIG_WRAPPER_HPP
#define CONFIG_WRAPPER_HPP

#include <cstdint>

#include "main.hpp"
#include "sx127x_config.hpp"
#include "SX127x.h"

#define SLAVE_DEVICE_STM32L051C8Tx
// #define SLAVE_DEVICE_STM32F446RE

// singleton class wraps the low-level STM32 hardware operations 
// needed to talk to the SX127x LoRa radio
class ConfigWrapper
{
  public:
	/// Return the singleton instance of the wrapper.
	static ConfigWrapper& GetInstance();

	ConfigWrapper& operator=(ConfigWrapper&&) = delete;
	ConfigWrapper& operator=(const ConfigWrapper&) = delete;

	ConfigWrapper (ConfigWrapper&&) = delete;
	ConfigWrapper (const ConfigWrapper&) = delete;

	~ConfigWrapper() = default;

	/// Pull the SX127x chip select line low.
	void csLow();

	/// Release the SX127x chip select line high.
	void csHigh();

	/// Initialize the wrapper with the SPI handle.
	void init(SPI_HandleTypeDef* spi);

	/// Pull the SX127x reset line low.
	void resetLow();

	/// Release the SX127x reset line high.
	void resetHigh();

	/// Delay execution for the requested number of milliseconds.
	void delayMs(std::uint32_t ms);

	/// Perform a single SPI transfer and return the received byte.
	std::uint8_t spiTransfer(std::uint8_t data);

	/// Read a single register from the SX127x.
	uint8_t readRegister(uint8_t addr);

	/// Write a single register on the SX127x.
	void writeRegister(uint8_t addr, uint8_t value);

  private:
    ConfigWrapper() = default;

    SPI_HandleTypeDef* spiHandle = nullptr;

};

#endif // CONFIG_WRAPPER_HPP
