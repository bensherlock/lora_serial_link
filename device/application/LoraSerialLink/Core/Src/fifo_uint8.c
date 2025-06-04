/**
 * @file    fifo_uint8.c
 *
 * @brief   FIFO for uint8.
 *
 * @copyright Copyright (c) 2025 Ben Sherlock
 *
 *  <multi-line description>
 *
 */


/*
 * Includes
 */
#include "fifo_uint8.h"

#include <stdint.h>


/*
 * Private: Constants and Macros 
 */

//#define U	(1)		/*!<  */




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



/*
 * Private: Function Prototypes/Declarations
 */



/*
 * Public: Function Definitions
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
int32_t fifo_uint8_init (fifo_uint8_state_t* fifo_state, uint32_t buffer_length, uint8_t buffer[buffer_length])
{
	fifo_state->read_idx = 0U;
	fifo_state->write_idx = 0U;
	fifo_state->buffer_length = buffer_length;
	fifo_state->buffer = buffer;

	return 0;
}


/**
 * @brief   Check if FIFO is empty.
 *
 * <optional long description>
 *
 * @param[int]    fifo_state pointer to fifo state struct
 * @return        1 for True, 0 for False
 */
int32_t fifo_uint8_is_empty (fifo_uint8_state_t* fifo_state)
{
	if (fifo_state->read_idx == fifo_state->write_idx)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/**
 * @brief   Check if FIFO is full.
 *
 * <optional long description>
 *
 * @param[int]    fifo_state pointer to fifo state struct
 * @return        1 for True, 0 for False
 */
int32_t fifo_uint8_is_full (fifo_uint8_state_t* fifo_state)
{
	if (fifo_state->read_idx == fifo_state->write_idx)
	{
		// Empty
		return 0;
	}
	else if ((fifo_state->read_idx == 0) && (fifo_state->write_idx == fifo_state->buffer_length-1))
	{
		// Full - write index would wrap onto the read index on write
		return 1;
	}
	else if ((fifo_state->write_idx < fifo_state->read_idx) && (fifo_state->write_idx == fifo_state->read_idx-1))
	{
		// Full - write index would increment onto the read index on write
		return 1;
	}

	// Not full
	return 0;
}


/**
 * @brief   Write one value to FIFO.
 *
 * <optional long description>
 *
 * @param[in/out]    fifo_state pointer to fifo state struct
 * @param[in]     the_byte the value to write to the fifo
 * @return        0 for success or Error
 */
int32_t fifo_uint8_write_one (fifo_uint8_state_t* fifo_state, uint8_t the_byte)
{
	if (fifo_uint8_is_full(fifo_state))
	{
		return -1; // Error, cannot write.
	}
	else
	{
		// Write into current write index, then increment the index
		fifo_state->buffer[fifo_state->write_idx] = the_byte;
		fifo_state->write_idx++;
		if (fifo_state->write_idx >= fifo_state->buffer_length)
		{
			// Wrap
			fifo_state->write_idx = 0;
		}

		return 0;
	}
}


/**
 * @brief   Read one value from FIFO.
 *
 * <optional long description>
 *
 * @param[in/out]    fifo_state pointer to fifo state struct
 * @param[out]     the_byte the value to read from the fifo
 * @return        0 for success or Error
 */
int32_t fifo_uint8_read_one (fifo_uint8_state_t* fifo_state, uint8_t* the_byte)
{
	if (fifo_uint8_is_empty(fifo_state))
	{
		return -1; // Error, cannot read
	}
	else
	{
		// Read from current index, then increment the index
		*the_byte = fifo_state->buffer[fifo_state->read_idx];
		fifo_state->read_idx++;
		if (fifo_state->read_idx >= fifo_state->buffer_length)
		{
			// Wrap
			fifo_state->read_idx = 0;
		}
	}
}





/*
 * Private: Function Definitions
 */



/* End of file */
