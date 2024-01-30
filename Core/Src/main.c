/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "cc1101.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);

#define PWM_SIZE 3
#define PWM_TRUE 0b011
#define PWM_FALSE 0b001
#define ARRAY_TO_BIT(array, bit, size) ((array[bit / (8 * size)] >> (((8 * size) - 1) - bit)) & 1)
#define PWM_BIT_ENCODE(bit) (bit ? PWM_TRUE : PWM_FALSE)

#define SET_BIT_ARRAY(out, index, value) (out[((index) - ((index) % 8)) / 8] |= (value&1) << (7-((index) % 8)))

void PWM_Encode(uint8_t* in , uint8_t* out, size_t size_in){
    for(size_t i = 0;i < size_in ;i++) {
        for (int8_t b=0;b<8;b++) {
            if (in[i] & (1 << (7 - b))) {
                SET_BIT_ARRAY(out, (i*8+b)*3   , (PWM_TRUE&0b100) >> 2);
                SET_BIT_ARRAY(out, (i*8+b)*3 +1, (PWM_TRUE&0b010) >> 1);
                SET_BIT_ARRAY(out, (i*8+b)*3 +2, (PWM_TRUE&0b001)     );
            } else {
                SET_BIT_ARRAY(out, (i*8+b)*3   , (PWM_FALSE&0b100) >> 2);
                SET_BIT_ARRAY(out, (i*8+b)*3 +1, (PWM_FALSE&0b010) >> 1);
                SET_BIT_ARRAY(out, (i*8+b)*3 +2, (PWM_FALSE&0b001)     );
            }
        }
    }
    
}

void nice_reformat(uint8_t* pwm_data)
{
	uint8_t tmp;
	uint8_t tmp2;

	// Adding manual 1 bit preamble
	tmp = pwm_data[0] & 1;
	pwm_data[0] = (pwm_data[0] >> 1) | (1 << 7);
	for (uint8_t i = 1; i < 5; i++) {
		tmp2 = pwm_data[i] & 1;
		pwm_data[i] = (pwm_data[i] >> 1) | (tmp << 7);
		tmp = tmp2;
	}

	// Triming useless bits
	pwm_data[4] &= (0xFF << 2);
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_SPI2_Init();
	MX_USB_DEVICE_Init();

	rfSettings settings = {
		0x29,  // IOCFG2        GDO2 Output Pin Configuration
		0x2E,  // IOCFG1        GDO1 Output Pin Configuration
		0x06,  // IOCFG0        GDO0 Output Pin Configuration
		0x07,  // FIFOTHR       RX FIFO and TX FIFO Thresholds
		0xD3,  // SYNC1         Sync Word, High Byte
		0x91,  // SYNC0         Sync Word, Low Byte
		0x5,   // PKTLEN        Packet Length
		0x04,  // PKTCTRL1      Packet Automation Control
		0x00,  // PKTCTRL0      Packet Automation Control
		0x00,  // ADDR          Device Address
		0x00,  // CHANNR        Channel Number
		0x06,  // FSCTRL1       Frequency Synthesizer Control
		0x00,  // FSCTRL0       Frequency Synthesizer Control
		0x10,  // FREQ2         Frequency Control Word, High Byte
		0xB0,  // FREQ1         Frequency Control Word, Middle Byte
		0x71,  // FREQ0         Frequency Control Word, Low Byte
		0xF5,  // MDMCFG4       Modem Configuration
		0xE9,  // MDMCFG3       Modem Configuration
		0x30,  // MDMCFG2       Modem Configuration
		0x20,  // MDMCFG1       Modem Configuration
		0x00,  // MDMCFG0       Modem Configuration
		0x15,  // DEVIATN       Modem Deviation Setting
		0x07,  // MCSM2         Main Radio Control State Machine Configuration
		0x30,  // MCSM1         Main Radio Control State Machine Configuration
		0x18,  // MCSM0         Main Radio Control State Machine Configuration
		0x16,  // FOCCFG        Frequency Offset Compensation Configuration
		0x6C,  // BSCFG         Bit Synchronization Configuration
		0x03,  // AGCCTRL2      AGC Control
		0x40,  // AGCCTRL1      AGC Control
		0x91,  // AGCCTRL0      AGC Control
		0x87,  // WOREVT1       High Byte Event0 Timeout
		0x6B,  // WOREVT0       Low Byte Event0 Timeout
		0xF8,  // WORCTRL       Wake On Radio Control
		0x56,  // FREND1        Front End RX Configuration
		0x11,  // FREND0        Front End TX Configuration
		0xE9,  // FSCAL3        Frequency Synthesizer Calibration
		0x2A,  // FSCAL2        Frequency Synthesizer Calibration
		0x00,  // FSCAL1        Frequency Synthesizer Calibration
		0x1F,  // FSCAL0        Frequency Synthesizer Calibration
		0x41,  // RCCTRL1       RC Oscillator Configuration
		0x00,  // RCCTRL0       RC Oscillator Configuration
		0x59,  // FSTEST        Frequency Synthesizer Calibration Control
		0x7F,  // PTEST         Production Test
		0x3F,  // AGCTEST       AGC Test
		0x81,  // TEST2         Various Test Settings
		0x35,  // TEST1         Various Test Settings
		0x09,  // TEST0         Various Test Settings
	};

	CC1101_HandleTypeDef hcc1101 = CC1101_Init(&hspi2, settings);
	hcc1101.settings = settings;
	hcc1101.ConfUpdate(&hcc1101);

	uint8_t volatile data[] = {0b01000111, 0b01100000};

	uint8_t volatile data_encode[2*PWM_SIZE] = {0};
	PWM_Encode(data, data_encode, 2);
	nice_reformat(data_encode);

	while (1)
	{
		HAL_Delay(15);

		hcc1101.SendPacket(&hcc1101, data_encode, 5);
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

	/** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
  */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* PB12 Enable in output mode */
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
