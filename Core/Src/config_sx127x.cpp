#include "config_sx127x.hpp"
#include "config_wrapper.hpp"

bool init_SX127x(ConfigWrapper& cfg_f)
{
	cfg_f.resetLow();
	cfg_f.delayMs(10);

	cfg_f.resetHigh();
	cfg_f.delayMs(10);

	uint8_t version = cfg_f.readRegister(0x42);

	if(version == 0x12)
	{

	    return true;
	}

	return false;
}

void sleep_mode(ConfigWrapper& cfg_f)
{

	cfg_f.writeRegister(REG_OP_MODE, LONG_RANGE_MODE | MODE_SLEEP);
}

void standby_mode(ConfigWrapper& cfg_f)
{

	cfg_f.writeRegister(REG_OP_MODE, LONG_RANGE_MODE | MODE_STDBY);
}

void sendPacket(ConfigWrapper& cfg, const std::uint8_t* data, uint8_t length)
{
	uint32_t start = HAL_GetTick();

    // Standby mode
    // cfg.writeRegister(0x01, 0x81);

    // FIFO pointer
    cfg.writeRegister(0x0D, 0x00);

    // Write payload
    for(uint8_t i = 0; i < length; i++)
    {
        cfg.writeRegister(0x00, data[i]);
    }

    // Payload length = 1 byte
    cfg.writeRegister(0x22, length);

    // TX mode
    cfg.writeRegister(0x01, 0x83);

    // Toggle LED to confirm TX starts
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    // Wait TX done
    while((cfg.readRegister(0x12) & 0x08) == 0) {
        if((HAL_GetTick() - start) > 1000) {
            return;
        }
    }

    // Toggle again to confirm TX completed
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    // Clear IRQ flag
    cfg.writeRegister(0x12, 0x08);
}

void sendPacketAM312(ConfigWrapper& cfg, const std::uint8_t* data, uint8_t length)
{
	uint32_t start = HAL_GetTick();

    // Standby mode
    cfg.writeRegister(0x01, 0x81);

    // FIFO pointer
    cfg.writeRegister(0x0D, 0x00);

    // Write payload
    for(uint8_t i = 0; i < length; i++)
    {
        cfg.writeRegister(0x00, data[i]);
    }

    // Payload length = 1 byte
    cfg.writeRegister(0x22, length);

    // TX mode
    cfg.writeRegister(0x01, 0x83);

    // Toggle LED to confirm TX starts
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    // Wait TX done
    while((cfg.readRegister(0x12) & 0x08) == 0) {
        if((HAL_GetTick() - start) > 1000) {
            return;
        }
    }

    // Toggle again to confirm TX completed
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    // Clear IRQ flag
    cfg.writeRegister(0x12, 0x08);
}

bool configureLoRa(ConfigWrapper& cfg)
{
    // LoRa sleep
    cfg.writeRegister(
        RADIOLIB_SX127X_REG_OP_MODE,
        RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_SLEEP
    );

    cfg.delayMs(10);

    // Frequency
    cfg.writeRegister(RADIOLIB_SX127X_REG_FRF_MSB, LORA_FREQUENCY_H);
    cfg.writeRegister(RADIOLIB_SX127X_REG_FRF_MID, LORA_FREQUENCY_M);
    cfg.writeRegister(RADIOLIB_SX127X_REG_FRF_LSB, LORA_FREQUENCY_L);

    // Power
    cfg.writeRegister(RADIOLIB_SX127X_REG_PA_CONFIG, LORA_PA_CONFIG);

    // Modem
    cfg.writeRegister(RADIOLIB_SX127X_REG_MODEM_CONFIG_1, LORA_MODEM_CONFIG_1);
    cfg.writeRegister(RADIOLIB_SX127X_REG_MODEM_CONFIG_2, LORA_MODEM_CONFIG_2);

    // Preamble
    cfg.writeRegister(RADIOLIB_SX127X_REG_PREAMBLE_MSB, LORA_PREAMBLE_MSB);
    cfg.writeRegister(RADIOLIB_SX127X_REG_PREAMBLE_LSB, LORA_PREAMBLE_LSB);

    // FIFO base addresses
    cfg.writeRegister(RADIOLIB_SX127X_REG_FIFO_TX_BASE_ADDR, 0x00);
    cfg.writeRegister(RADIOLIB_SX127X_REG_FIFO_RX_BASE_ADDR, 0x00);

    // Clear IRQs
    cfg.writeRegister(RADIOLIB_SX127X_REG_IRQ_FLAGS, 0xFF);

    // synch word 0xF4
    cfg.writeRegister(0x39, LORA_SYNC_WORD);

    // Standby
    cfg.writeRegister(
        RADIOLIB_SX127X_REG_OP_MODE,
        RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_STANDBY
    );

    return true;
}