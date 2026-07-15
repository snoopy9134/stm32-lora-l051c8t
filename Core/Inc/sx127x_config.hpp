/*
 * sx127x_config.hpp
 *
 *  Created on: Jun 4, 2026
 *      Author: vboxuser
 */

#ifndef INC_SX127X_CONFIG_HPP
#define INC_SX127X_CONFIG_HPP

///************* 868 MHz ****************/
//#define LORA_FREQUENCY_H  		0xD9
//#define LORA_FREQUENCY_M  		0x00
//#define LORA_FREQUENCY_L  		0x00

/************* 434 MHz ****************/
#define LORA_FREQUENCY_H 0x6C
#define LORA_FREQUENCY_M 0x40
#define LORA_FREQUENCY_L 0x00

/************* 915 MHz ****************/
/*
#define LORA_FREQUENCY_H 0xE4
#define LORA_FREQUENCY_M 0xC0
#define LORA_FREQUENCY_L 0x00
*/

#define LORA_PA_CONFIG    		0x8F

#define LORA_MODEM_CONFIG_1    	0x72
#define LORA_MODEM_CONFIG_2    	0x74

#define LORA_PREAMBLE_MSB 		0x00
#define LORA_PREAMBLE_LSB 		0x08

#define LORA_SYNC_WORD    		0x12


#endif /* INC_SX127X_CONFIG_HPP */
