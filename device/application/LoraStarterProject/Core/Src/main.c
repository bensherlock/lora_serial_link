/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dbg_output.h"
#include "rfm95w.h"
#include "fifo_uint8.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* Definition of the LoRa Packet Structures */
typedef struct lora_packet_header_t_
{
	uint8_t destination_address;
	uint8_t source_address;
	uint8_t sequence_number;
	uint8_t ctrl_and_retry_count;
} lora_packet_header_t;

#define LORA_PACKET_MAX_PAYLOAD	(250U)

typedef struct lora_packet_t_
{
	lora_packet_header_t header;
	uint8_t payload[LORA_PACKET_MAX_PAYLOAD];
	uint32_t payload_length;
} lora_packet_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MAIN_STRING_BUFFER_MAXLEN	(1024)

#define UART_FIFO_BUFFER_SIZE	(2048U)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t g_main_string_buffer[MAIN_STRING_BUFFER_MAXLEN] = {0U}; /* Buffer for formatted strings */
static uint32_t g_main_string_buffer_length = 0U;

static volatile uint32_t g_main_half_second_counter = 0U; /*!< Counter incremented by TIM15 callback at 2Hz */
static volatile uint64_t g_main_millisecond_counter = 0U; /*!< Counter incremented by TIM16 callback at 1kHz */



/* UART Transmit Variables */
static volatile fifo_uint8_state_t g_uart_transmit_fifo = {0};
static volatile uint8_t g_uart_transmit_fifo_buffer[UART_FIFO_BUFFER_SIZE] = {0};
static volatile uint8_t g_uart_transmit_fifo_in_process = 0;

/* UART Receive Variables */
static volatile fifo_uint8_state_t g_uart_receive_fifo = {0};
static volatile uint8_t g_uart_receive_fifo_buffer[UART_FIFO_BUFFER_SIZE] = {0};
static volatile uint8_t g_uart_receive_fifo_in_process = 0;
static volatile uint8_t g_main_serial_byte_to_receive = 0;
static volatile uint64_t g_main_last_received_serial_byte_time_ms = 0;

/* Lora packet variables*/
static lora_packet_t g_lora_packet_to_transmit = {0};
static lora_packet_t g_lora_packet_received = {0};
static uint64_t g_lora_packet_transmit_serial_timeout = 200; // 200ms

static uint8_t g_lora_source_address = 0; // Set these manually for now
static uint8_t g_lora_destination_address = 255; // Set these manually for now
static uint8_t g_lora_broadcast_address = 255;

static uint8_t g_lora_sequence_number = 0;
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
  MX_TIM15_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_LPUART1_UART_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1U); // Light the LED

  HAL_TIM_Base_Start_IT(&htim15); // Start the timer TIM15 which will toggle the LED
  HAL_TIM_Base_Start_IT(&htim16); // Start the timer TIM16 for ms counter

  dbg_output_init(&hlpuart1); // Initialise the debug stream using LPUART1


  // Initialise the UART FIFOs
  fifo_uint8_init(&g_uart_transmit_fifo, UART_FIFO_BUFFER_SIZE, g_uart_transmit_fifo_buffer);
  fifo_uint8_init(&g_uart_receive_fifo, UART_FIFO_BUFFER_SIZE, g_uart_receive_fifo_buffer);


  // Send Message To Debug UART
  dbg_output_write_str("HelloWorld\r\n");


  // Initialise the RFM95W
  rfm95w_init(&hspi1);


  // Send Test Packet
  g_main_string_buffer_length = sprintf(&g_main_string_buffer[0], "HelloWorld\r\n");
  rfm95w_transmit_packet(g_main_string_buffer_length, &g_main_string_buffer[0]);


  // start listening
  rfm95w_listen_for_packets();

  // Start the UART1 Serial listening for incoming bytes
  HAL_UART_Receive_IT(&huart1, &g_main_serial_byte_to_receive, 1);
  g_uart_receive_fifo_in_process = 1;


  // Initialise the header in the packet for transmitting
  g_lora_packet_to_transmit.header.source_address = g_lora_source_address;
  g_lora_packet_to_transmit.header.destination_address = g_lora_destination_address;
  g_lora_packet_to_transmit.header.sequence_number = g_lora_sequence_number;
  g_lora_sequence_number++; // increment for the next packet

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  // 1
	  // Check for Received packet flag
	  // Get the received packet into the packet structure (set the payload length)
	  // clear the received packet flag
	  // check the received packet header - our address or broadcast address
	  // If is for us then put the payload bytes into the serial transmit fifo
	  // if serial transmit is not currently in progress then kick it off


	  //2
	  // Check for serial bytes in the receive fifo
	  // take each byte and add into current transmit packet payload
	  // if we have reached the max payload length then transmit the packet and then clear the structures and start afresh


	  // 3
	  // Check for timeout since last serial byte received.
	  // If we currently have a packet becing assembled for transmission then transmit the packet and then clear the structures and start afresh









    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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


/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM15)
	{
		g_main_half_second_counter++; // Increment the 2Hz counter
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}

	if (htim->Instance == TIM16)
	{
		g_main_millisecond_counter++; // Increment the counter
	}
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RFM95W_G0_GPIO_PIN)
	{
		// Interrupt from RFM95W
		rfm95w_process_interrupt();
	}
}

/**
  * @brief Tx Transfer completed callback.
  * @param huart UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	// Debug Serial
	if (huart->Instance == LPUART1)
	{
		dbg_output_process_on_interrupt();
	}

	// Main Serial
	if (huart->Instance == USART1)
	{
		// Check transmit fifo to continue transmissions
		if (fifo_uint8_is_empty(&g_uart_transmit_fifo) == 1)
		{
			// Transmission complete as fifo is empty
			g_uart_transmit_fifo_in_process = 0;
		}
		else
		{
			// Send the next byte
			uint8_t next_byte = 0;
			fifo_uint8_read_one(&g_uart_transmit_fifo, &next_byte);
			HAL_UART_Transmit_IT(&huart1, &next_byte, 1);
			g_uart_transmit_fifo_in_process = 1;
		}
	}
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) // BS: USing the other port as shortage of cables
	{
		// just received data into g_main_serial_byte_to_receive
		// put into fifo so it can be processed by the main loop to construc a lora packet to transmit.
		fifo_uint8_write_one(&g_uart_receive_fifo, g_main_serial_byte_to_receive);

		// last byte received at:
		g_main_last_received_serial_byte_time_ms = g_main_millisecond_counter;


		// Start another receive
		HAL_UART_Receive_IT(&huart1, &g_main_serial_byte_to_receive, 1);
		g_uart_receive_fifo_in_process = 1;
	}
}

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
