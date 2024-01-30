/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "vu_meter.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CLI_BUFFER_LENGTH 40
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static volatile uint8_t uart2_data;
static h_vu_t h_vu;
static char cli_buffer[CLI_BUFFER_LENGTH];
static uint32_t cli_it = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *) ptr, (uint16_t)len, HAL_MAX_DELAY);

	return len;
}

void cli_vu_meter(char * buffer)
{
	int i = 2;
	int port;
	int led;
	int value;

	// Lecture du port
	if (buffer[i] == '0')
	{
		port = 0;
	}
	else if (buffer[i] == '1')
	{
		port = 1;
	}
	else
	{
		printf("Error\r\n");
		return;
	}

	i += 2;

	// Lecture de la LED
	char * ptr;
	ptr = &(buffer[i]);

	while(buffer[i] != ' ')
	{
		i++;
	}

	buffer[i] = '\0';

	led = atoi(ptr);

	i++;

	if (buffer[i] == '0')
	{
		value = 0;
	}
	else if (buffer[i] == '1')
	{
		value = 1;
	}
	else
	{
		printf("Error\r\n");
		return;
	}

	printf("port = %d\r\n", port);
	printf("led = %d\r\n", led);
	printf("value = %d\r\n", value);

	vu_led(&h_vu, port, led, value);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Appui sur entrée = 13
	if (uart2_data != 13)
	{
		if (cli_it < CLI_BUFFER_LENGTH - 1)
			// - 1 parce qu'on laisse de la place pour le caractère '\0'
		{
			HAL_UART_Transmit(&huart2, &uart2_data, 1, HAL_MAX_DELAY);
			cli_buffer[cli_it] = uart2_data;
			cli_it++;
		}
	}
	else
	{
		cli_buffer[cli_it] = '\0';	// Caractère '\0' obligatoire pour bien terminer la chaine de caractères
		printf("\r\n:%s\r\n", cli_buffer);

		switch(cli_buffer[0])
		{
		case 'v':
			cli_vu_meter(cli_buffer);
			break;
		default:
			printf("Unknown command\r\n");
			break;
		}

		cli_it = 0;	// Pour redémarrer à 0
		printf("> ");
		fflush(stdout);
		for (int i = 0 ; i < CLI_BUFFER_LENGTH ; i++)
		{
			// Vider le buffer pour plus de sécurité
			cli_buffer[i] = 0;
		}
	}

	HAL_UART_Receive_IT(&huart2, &uart2_data, 1);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
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
	MX_USART2_UART_Init();
	MX_SPI3_Init();
	/* USER CODE BEGIN 2 */
	printf("\r\n=============================================================\r\n");
	printf("> ");
	fflush(stdout);

	vu_init(&h_vu, &hspi3);
	vu_blink(&h_vu);

	HAL_UART_Receive_IT(&huart2, &uart2_data, 1);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
#ifdef UART_POLLING
		uint8_t valeur;
		HAL_UART_Receive(&huart2, &valeur, 1, 1000);
		HAL_UART_Transmit(&huart2, &valeur, 1, HAL_MAX_DELAY);
#endif //UART_POLLING

#ifdef LED
		if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
		{
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			HAL_Delay(100);	// 100ms : 10Hz
		}
		else
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}
#endif //LED
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		// Ou là !
	}

	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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

	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
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
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
