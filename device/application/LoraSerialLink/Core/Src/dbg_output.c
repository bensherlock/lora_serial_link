/**
 * @file    dbg_output.c
 *
 * @brief   Debug Output Module.
 *
 * @copyright Copyright (c) 2025 Ben Sherlock
 *
 *  <multi-line description>
 *
 */


/*
 * Includes
 */
#include "dbg_output.h"

#include "fifo_uint8.h"

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_uart.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>


/*
 * Private: Constants and Macros 
 */

//#define U	(1)		/*!<  */
#define TRANSMIT_BUFFER_SIZE (128U)

#define TRANSMIT_FIFO_BUFFER_SIZE	(2048U)


/*
 * Public: Opaque Type Definitions
 */


/*
 * Private: Typedefs
 */



/*
 * Public: Constants
 */


/*
 * Public: Variables
 */


/*
 * Private: Constants
 */



/*
 * Private: Variables
 */
static uint8_t g_initialised = 0;           /*!< Initialised flag */


static UART_HandleTypeDef* g_phuart;


/* Blocking UART transmit */
static uint8_t g_transmit_buffer[TRANSMIT_BUFFER_SIZE] = { 0U };
static uint32_t g_transmit_length = 0;


/* Non-blocking UART transmit */
static volatile fifo_uint8_state_t g_transmit_fifo = {0};
static volatile uint8_t g_transmit_fifo_buffer[TRANSMIT_FIFO_BUFFER_SIZE] = {0};
static volatile uint8_t g_transmit_fifo_in_process = 0;

/*
 * Private: Function Prototypes/Declarations
 */



/*
 * Public: Function Definitions
 */

/**
 * @brief   Initialise Debug Output module.
 *
 * @param[in]     phuart pointer to handle of uart to write to
 * @return        0 for success or Error
 */
int32_t dbg_output_init(UART_HandleTypeDef* phuart)
{
	g_phuart = phuart;
	memset(&g_transmit_buffer[0], 0, TRANSMIT_BUFFER_SIZE*sizeof(uint8_t));

	fifo_uint8_init(&g_transmit_fifo, TRANSMIT_FIFO_BUFFER_SIZE, g_transmit_fifo_buffer);

    g_initialised = 1;

    return (0);
}


/**
 * @brief   Write a buffer to the debug output  - blocking.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_buffer_blocking (uint32_t buffer_length, uint8_t buffer[buffer_length])
{
	HAL_UART_Transmit(g_phuart, buffer, buffer_length, 100);

	return (0);
}


/**
 * @brief   Write a string to the debug output stream - blocking.
 *
 * <optional long description>
 *
 * @param[in]     str pointer to start of null terminated string
 * @return        0 for success or Error
 */
int32_t dbg_output_write_str_blocking (char* str)
{
	g_transmit_length = sprintf(g_transmit_buffer, str);
	HAL_UART_Transmit(g_phuart, g_transmit_buffer, g_transmit_length, 100);

	return (0);
}


/**
 * @brief   Write a buffer to the debug output  - non-blocking.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_buffer (uint32_t buffer_length, uint8_t buffer[buffer_length])
{
	// append to fifo
	for (uint32_t idx = 0; idx < buffer_length; idx++)
	{
		// No checks for fullness at the moment.
		fifo_uint8_write_one(&g_transmit_fifo, buffer[idx]);
	}

	// if transmission not in process then kick it off
	if (g_transmit_fifo_in_process == 0)
	{
		// Send the next byte
		uint8_t next_byte = 0;
		fifo_uint8_read_one(&g_transmit_fifo, &next_byte);
		HAL_UART_Transmit_IT(g_phuart, &next_byte, 1);
		g_transmit_fifo_in_process = 1;
	}


	return (0);
}

/**
 * @brief   Write a buffer to the debug output stream as hex encoded strings with csv - non-blocking.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_hex_encoded_csv_buffer (uint32_t buffer_length, uint8_t buffer[buffer_length])
{
	char string_buffer[10];
	uint32_t string_length = 0;


	for (uint32_t idx = 0; idx < buffer_length; idx++)
	{
		// Generate formatted string
		string_length = sprintf(&string_buffer[0], "0x%02X,", buffer[idx]);

		// append to fifo
		for (uint32_t ch_idx = 0; ch_idx < string_length; ch_idx++)
		{
			// No checks for fullness at the moment.
			fifo_uint8_write_one(&g_transmit_fifo, string_buffer[ch_idx]);
		}
	}

	// if transmission not in process then kick it off
	if (g_transmit_fifo_in_process == 0)
	{
		// Send the next byte
		uint8_t next_byte = 0;
		fifo_uint8_read_one(&g_transmit_fifo, &next_byte);
		HAL_UART_Transmit_IT(g_phuart, &next_byte, 1);
		g_transmit_fifo_in_process = 1;
	}


	return (0);
}

/**
 * @brief   Write a string to the debug output stream - non-blocking.
 *
 * <optional long description>
 *
 * @param[in]     str pointer to start of null terminated string
 * @return        0 for success or Error
 */
int32_t dbg_output_write_str (char* str)
{
	// append to fifo
	uint32_t string_length = strlen(str);
	for (uint32_t idx = 0; idx < string_length; idx++)
	{
		// No checks for fullness at the moment.
		fifo_uint8_write_one(&g_transmit_fifo, str[idx]);
	}

	// if transmission not in process then kick it off
	if (g_transmit_fifo_in_process == 0)
	{
		// Send the next byte
		uint8_t next_byte = 0;
		fifo_uint8_read_one(&g_transmit_fifo, &next_byte);
		HAL_UART_Transmit_IT(g_phuart, &next_byte, 1);
		g_transmit_fifo_in_process = 1;
	}

	return (0);
}


/**
 * @brief   Called by the Interrupt Callback to process the FIFO transmissions.
 *
 * <optional long description>
 *
 * @param         None
 * @return        0 for success or Error
 */
int32_t dbg_output_process_on_interrupt()
{

	if (fifo_uint8_is_empty(&g_transmit_fifo) == 1)
	{
		// Transmission complete as fifo is empty
		g_transmit_fifo_in_process = 0;
	}
	else
	{
		// Send the next byte
		uint8_t next_byte = 0;
		fifo_uint8_read_one(&g_transmit_fifo, &next_byte);
		HAL_UART_Transmit_IT(g_phuart, &next_byte, 1);
		g_transmit_fifo_in_process = 1;
	}

	return 0;
}


/*
 * Private: Function Definitions
 */


/* End of file */
