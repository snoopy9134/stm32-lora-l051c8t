#include "config_sx127x.hpp"
#include "config_wrapper.hpp"
#include "protocol.hpp"

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

	cfg_f.writeRegister(RADIOLIB_SX127X_REG_OP_MODE, RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_SLEEP);
}

void standby_mode(ConfigWrapper& cfg_f)
{

	cfg_f.writeRegister(RADIOLIB_SX127X_REG_OP_MODE, RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_STANDBY);
}

void sendPacket(ConfigWrapper& cfg, const std::uint8_t* data, uint8_t length)
{
	uint32_t start = HAL_GetTick();

    // Standby mode
    cfg.writeRegister(RADIOLIB_SX127X_REG_OP_MODE, RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_STANDBY);

    // FIFO pointer
    cfg.writeRegister(RADIOLIB_SX127X_REG_FIFO_ADDR_PTR, 0x00);

    // Write payload
    for(uint8_t i = 0; i < length; i++)
    {
        cfg.writeRegister(RADIOLIB_SX127X_REG_FIFO, data[i]);
    }

    // Payload length = 1 byte
    cfg.writeRegister(RADIOLIB_SX127X_REG_PAYLOAD_LENGTH, length);

    // TX mode
    cfg.writeRegister(RADIOLIB_SX127X_REG_OP_MODE, RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_TX);

    // Wait TX done
    while((cfg.readRegister(RADIOLIB_SX127X_REG_IRQ_FLAGS) & RADIOLIB_SX127X_CLEAR_IRQ_FLAG_TX_DONE) == 0) {
        if((HAL_GetTick() - start) > 1000) {
            return;
        }
    }

    // Clear IRQ flag
    cfg.writeRegister(RADIOLIB_SX127X_REG_IRQ_FLAGS, RADIOLIB_SX127X_CLEAR_IRQ_FLAG_TX_DONE);
}

bool waitForAck(ConfigWrapper& cfg,
                uint32_t timeoutMs,
                const uint8_t expectedCounter)
{    
    cfg.writeRegister(RADIOLIB_SX127X_REG_OP_MODE,
                  RADIOLIB_SX127X_LORA | RADIOLIB_SX127X_RXSINGLE);
    
    uint32_t start = HAL_GetTick();

    while((HAL_GetTick() - start) < timeoutMs)
    {
        uint8_t irq = cfg.readRegister(RADIOLIB_SX127X_REG_IRQ_FLAGS);

        if(irq & RADIOLIB_SX127X_CLEAR_IRQ_FLAG_RX_DONE)
        {
            return verifyAckData(cfg, expectedCounter);
        }

        if (irq & RADIOLIB_SX127X_CLEAR_IRQ_FLAG_PAYLOAD_CRC_ERROR)
        {
            // Packet corrupted
        }
    }

    return false; // Timeout or no ACK received
}

bool verifyAckData(ConfigWrapper& cfg, const uint8_t expectedCounter)
{
    std::size_t length = cfg.readRegister(RADIOLIB_SX127X_REG_RX_NB_BYTES);

    if (length != sizeof(RxDataProtocol))
    {
        return false; // Unexpected packet size
    }

    uint8_t fifoAddr =
    cfg.readRegister(RADIOLIB_SX127X_REG_FIFO_RX_CURRENT_ADDR);

    cfg.writeRegister(RADIOLIB_SX127X_REG_FIFO_ADDR_PTR, fifoAddr);

    std::uint8_t rxBuffer[sizeof(RxDataProtocol)] = {0};

    for (std::size_t i = 0; i < length; i++)
    {
        rxBuffer[i] = cfg.readRegister(RADIOLIB_SX127X_REG_FIFO);
    }

    RxDataProtocol* rxData = reinterpret_cast<RxDataProtocol*>(rxBuffer);

    if (rxData->version_major == VERSION_MAJOR && rxData->version_minor == VERSION_MINOR)
    {
        if (rxData->m_counter == expectedCounter)
        {
            return true; // ACK received with expected counter
        }
    }

    return false;
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