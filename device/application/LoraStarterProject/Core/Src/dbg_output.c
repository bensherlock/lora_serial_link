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
static uint8_t g_transmit_buffer[TRANSMIT_BUFFER_SIZE] = { 0U };
static uint32_t g_transmit_length = 0;


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

    g_initialised = 1;

    return (0);
}


/**
 * @brief   Write a buffer to the debug output stream.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_buffer (uint32_t buffer_length, uint8_t buffer[buffer_length])
{
	HAL_UART_Transmit(g_phuart, buffer, buffer_length, 100);

	return (0);
}


/**
 * @brief   Write a string to the debug output stream.
 *
 * <optional long description>
 *
 * @param[in]     str pointer to start of null terminated string
 * @return        0 for success or Error
 */
int32_t dbg_output_write_str (char* str)
{
	g_transmit_length = sprintf(g_transmit_buffer, str);
	HAL_UART_Transmit(g_phuart, g_transmit_buffer, g_transmit_length, 100);

	return (0);
}


/*
 * Private: Function Definitions
 */



/* End of file */
