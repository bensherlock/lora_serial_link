/**
 * @file    fifo_uint8.h
 *
 * @brief   FIFO for uint8.
 *
 * @copyright Copyright (c) 2025 Ben Sherlock
 *
 *  <multi-line description>
 *
 */

#ifndef FIFO_UINT8_H
#define FIFO_UINT8_H

/*
 * Includes
 */
#include <stdint.h>



/*
 * Public: Constants and Macros 
 */



/*
 * Public: Typedefs
 */

/**
 * @brief   Struct to hold the state of the FIFO instance.
 *
 * FIFO is empty when read and write indexes are equal. FIFO write will fail if FIFO is full.
 *
 */
typedef struct fifo_uint8_state_t_
{
	volatile uint32_t read_idx;			/*!< Read index */
	volatile uint32_t write_idx;			/*!< Write index */
	uint32_t buffer_length;		/*!< Length of the buffer */
	volatile uint8_t* buffer;			/*!< Pointer to Buffer for FIFO */

} fifo_uint8_state_t;



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
 * @brief   Initialise FIFO State.
 *
 * <optional long description>
 *
 * @param[out]    fifo_state pointer to fifo state struct
 * @param[in]     buffer_length of the provided buffer
 * @param[in]     buffer of length buffer_length for the fifo
 * @return        0 for success or Error
 */
int32_t fifo_uint8_init (fifo_uint8_state_t* fifo_state, uint32_t buffer_length, uint8_t buffer[buffer_length]);



/**
 * @brief   Check if FIFO is empty.
 *
 * <optional long description>
 *
 * @param[int]    fifo_state pointer to fifo state struct
 * @return        1 for True, 0 for False
 */
int32_t fifo_uint8_is_empty (fifo_uint8_state_t* fifo_state);


/**
 * @brief   Check if FIFO is full.
 *
 * <optional long description>
 *
 * @param[int]    fifo_state pointer to fifo state struct
 * @return        1 for True, 0 for False
 */
int32_t fifo_uint8_is_full (fifo_uint8_state_t* fifo_state);


/**
 * @brief   Write one value to FIFO.
 *
 * <optional long description>
 *
 * @param[in/out]    fifo_state pointer to fifo state struct
 * @param[in]     the_byte the value to write to the fifo
 * @return        0 for success or Error
 */
int32_t fifo_uint8_write_one (fifo_uint8_state_t* fifo_state, uint8_t the_byte);


/**
 * @brief   Read one value from FIFO.
 *
 * <optional long description>
 *
 * @param[in/out]    fifo_state pointer to fifo state struct
 * @param[out]     the_byte the value to read from the fifo
 * @return        0 for success or Error
 */
int32_t fifo_uint8_read_one (fifo_uint8_state_t* fifo_state, uint8_t* the_byte);


#endif /* FIFO_UINT8_H */

/* End of file */
