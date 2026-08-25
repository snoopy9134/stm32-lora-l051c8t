/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "gpio.hpp"
#include "main.hpp"
#include "adc.hpp"
#include "spi.hpp"
#include "lptim.hpp"
#include "config_wrapper.hpp"
#include "config_sx127x.hpp"
#include "protocol.hpp"

volatile bool pirDetected = false;
volatile bool lowPowerMode = false;
volatile bool lptimRunning = false;
volatile std::uint8_t dummyCnt = 0;
volatile uint8_t tx_counter = 0;

constexpr std::uint8_t DETECTION_TRIGGER = 0x03;

// Device specific
constexpr std::uint8_t DEVICE_ID = 0x01;

// cliabrated reference value for VREFINT (voltage measurements)
#define VREFINT_CAL_ADDR_PTR ((uint16_t*)0x1FF80078)


// Create data instance to transmit
DataProtocol dataProtocol(VERSION_MAJOR, VERSION_MINOR, DEVICE_ID);

uint8_t dataBuffer[8];

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

extern "C" int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_LPTIM1_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  [[maybe_unused]]ConfigWrapper& cfg = ConfigWrapper::GetInstance();

  cfg.init(&hspi2);

  [[maybe_unused]]auto init_state = init_SX127x(cfg);

  [[maybe_unused]]bool result_cfg_lora = false;


    if(init_state)
    {
  	  result_cfg_lora = configureLoRa(cfg);
    }
    else
    {
        while (1)  // ← ADD THIS!
        {
        }

    }

    if(result_cfg_lora)
    {
    }
    else
    {
  	  while (1)
  	  {
  	  }
    }

    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
    HAL_Delay(1000);
    
    [[maybe_unused]]bool sendValue = false;

    if(result_cfg_lora)
    {
  	   

  	  while(1)
  	  {
        dummyCnt++;

  		  if(pirDetected)
  		  {

          pirDetected = false;

          // reset timer only is running and start again (rolling timer)
          // with prescaled Div4 -> 5 seconds timeout
          if (lptimRunning == true)
          {
              HAL_LPTIM_TimeOut_Stop_IT(&hlptim1);
          }

          HAL_LPTIM_TimeOut_Start_IT(&hlptim1, 0xFFFF, 46250);
          lptimRunning = true;

          tx_counter++;

          if(DETECTION_TRIGGER <= tx_counter)
          {

            tx_counter = 0;

            HAL_LPTIM_TimeOut_Stop_IT(&hlptim1);
            lptimRunning = false;

            dataProtocol.setCounter(dummyCnt);
            dataProtocol.setMessageType(MessageType::MESSAGE);

             // trigger Vrefin measurement
            uint16_t Vdd_measured = triggerVDDMeasurement();
            uint16_t vrefint_cal = *VREFINT_CAL_ADDR_PTR;
            uint32_t vdd_mV = (3000UL * vrefint_cal / Vdd_measured);
            uint16_t voltage = static_cast<uint16_t>(vdd_mV);
            dataProtocol.setVoltage(voltage);

            dataProtocol.serialize(dataBuffer);

            // TxRx module into standby
            standby_mode(cfg);

            sendPacket(cfg, dataBuffer, sizeof(dataBuffer));

            uint32_t rxTimeout = 300;

            if(waitForAck(cfg, rxTimeout, dummyCnt))
            {
              HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
  			      HAL_Delay(500);
              HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
  			      HAL_Delay(500); 
              // success
            }
            else
            {
                // retransmit
            }

            // TxRx module to sleep
            sleep_mode(cfg);

          }

          // LED
          HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
  			  HAL_Delay(100);  

  		  }

  				enterStopMode();

  	  }

    }
    else
    {
  	  while(1)
  	  {
  	  }
    }
}

void enterStopMode()
{
	lowPowerMode = true;
	HAL_SuspendTick();
  // Clear the wake-up flag before entering Stop
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  // proceed with code execution after wake up
	SystemClock_Config();   // <-- Required
	HAL_ResumeTick();

	lowPowerMode = false;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_1)
	{
		pirDetected = true;
	}
}

extern "C" void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
  if (hlptim->Instance == LPTIM1)
  {
    // 5-second window expired
    tx_counter = 0;
    lptimRunning = false;
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.LptimClockSelection = RCC_LPTIM1CLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
