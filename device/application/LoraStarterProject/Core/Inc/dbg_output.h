/**
 * @file    dbg_output.h
 *
 * @brief   Debug Output Module.
 *
 * @copyright Copyright (c) 2025 Ben Sherlock
 *
 *  <multi-line description>
 *
 */

#ifndef DBG_OUTPUT_H
#define DBG_OUTPUT_H

/*
 * Includes
 */
#include <stdint.h>

#include "stm32l4xx_hal.h"


/*
 * Public: Constants and Macros 
 */



/*
 * Public: Typedefs
 */




/*
 * Public: Opaque Type Declarations
 */


/*
 * Public: Constants
 */


/*
 * Public: Variables (Avoid global variables if possible)
 */


/*
 * Public: Function Prototypes/Declarations
 */


/**
 * @brief   Initialise Debug Output Module.
 *
 * <optional long description>
 *
 * @param[in]     huart pointer to handle of uart to write to
 * @return        0 for success or Error
 */
int32_t dbg_output_init (UART_HandleTypeDef* phuart);


/**
 * @brief   Write a buffer to the debug output stream - non-blocking.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_buffer (uint32_t buffer_length, uint8_t buffer[buffer_length]);

/**
 * @brief   Write a buffer to the debug output stream as hex encoded strings with csv - non-blocking.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_hex_encoded_csv_buffer (uint32_t buffer_length, uint8_t buffer[buffer_length]);


/**
 * @brief   Write a string to the debug output stream - non-blocking.
 *
 * <optional long description>
 *
 * @param[in]     str pointer to start of null terminated string
 * @return        0 for success or Error
 */
int32_t dbg_output_write_str (char* str);


/**
 * @brief   Write a buffer to the debug output stream - blocking.
 *
 * <optional long description>
 *
 * @param[in]     buffer_length of buffer to send
 * @param[in]     buffer to send
 * @return        0 for success or Error
 */
int32_t dbg_output_write_buffer_blocking (uint32_t buffer_length, uint8_t buffer[buffer_length]);


/**
 * @brief   Write a string to the debug output stream - blocking.
 *
 * <optional long description>
 *
 * @param[in]     str pointer to start of null terminated string
 * @return        0 for success or Error
 */
int32_t dbg_output_write_str_blocking (char* str);


/**
 * @brief   Called by the Interrupt Callback to process the FIFO transmissions.
 *
 * <optional long description>
 *
 * @param         None
 * @return        0 for success or Error
 */
int32_t dbg_output_process_on_interrupt();



#endif /* DBG_OUTPUT_H */

/* End of file */
