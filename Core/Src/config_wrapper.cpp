/*
 * config_wrapper.cpp
 *
 *  Created on: May 27, 2026
 *      Author: vboxuser
 */
#include "config_wrapper.hpp"


ConfigWrapper& ConfigWrapper::GetInstance()
{
	static ConfigWrapper config_wrapper_singleton;

    return config_wrapper_singleton;
}

void ConfigWrapper::init(SPI_HandleTypeDef* spi)
{
    spiHandle = spi;
}

void ConfigWrapper::csLow()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}

void ConfigWrapper::csHigh()
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

void ConfigWrapper::resetLow()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  // PA8
}

void ConfigWrapper::resetHigh()
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);    // PA8
}

void ConfigWrapper::delayMs(std::uint32_t ms)
{
    HAL_Delay(ms);
}

uint8_t ConfigWrapper::readRegister(uint8_t addr)
{
    csLow();

    // SX127x read operation:
    // MSB = 0
    uint8_t reg = addr & 0x7F;

    spiTransfer(reg);

    uint8_t value = spiTransfer(0x00);

    csHigh();

    return value;
}

void ConfigWrapper::writeRegister(uint8_t addr, uint8_t value)
{
    csLow();

    // SX127x write operation:
    // MSB = 1
    spiTransfer(addr | 0x80);
    spiTransfer(value);

    csHigh();
}

uint8_t ConfigWrapper::spiTransfer(std::uint8_t data)
{
    std::uint8_t rx;

    HAL_SPI_TransmitReceive(
        spiHandle,
        &data,
        &rx,
        1,
        HAL_MAX_DELAY
    );

    return rx;
}



