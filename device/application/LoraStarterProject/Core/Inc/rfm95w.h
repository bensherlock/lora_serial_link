/**
 * @file    rfm95w.h
 *
 * @brief   LoRa RFM95W Module.
 *
 * @copyright Copyright (c) 2025 Ben Sherlock
 *
 *  <multi-line description>
 *
 */

#ifndef RFM95W_H
#define RFM95W_H

/*
 * Includes
 */
#include <stdint.h>

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"

/*
 * Public: Constants and Macros 
 */

#define RFM95W_EN_GPIO_PIN 		GPIO_PIN_0		/*!< EN pin */
#define RFM95W_EN_GPIO_PORT 	GPIOA			/*!< EN port */
#define RFM95W_G0_GPIO_PIN 		GPIO_PIN_2		/*!< G0 pin */
#define RFM95W_G0_GPIO_PORT 	GPIOA			/*!< G0 port */
#define RFM95W_RST_GPIO_PIN 	GPIO_PIN_3		/*!< RST pin */
#define RFM95W_RST_GPIO_PORT 	GPIOA			/*!< RST port */
#define RFM95W_CS_GPIO_PIN 		GPIO_PIN_4		/*!< CS pin */
#define RFM95W_CS_GPIO_PORT 	GPIOA			/*!< CS port */

#define RFM95W_FREQ_RF			(868000000.0f)	/*!< RF Centre Frequency (868 MHz) */

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
 * @brief   Initialise RFM95W module.
 *
 * @param[in]     spi_handle
 * @return        0 for success or Error
 */
int32_t rfm95w_init(SPI_HandleTypeDef* spi_handle);


/**
 * @brief   Transmit a LoRa Packet with the RFM95W module.
 *
 * @param[in]	  buffer_length	length of the buffer to transmit.
 * @param[in]	  buffer buffer to transmit.
 * @return        0 for success or Error
 */
int32_t rfm95w_transmit_packet(uint8_t buffer_length, uint8_t buffer[buffer_length]);



/**
 * @brief   Listen for incoming LoRa Packets with the RFM95W module.
 *
 * @param	      None
 * @return        0 for success or Error
 */
int32_t rfm95w_listen_for_packets();



/**
 * @brief   Has a packet been received by the RFM95W module.
 *
 * @param	      None
 * @return        1 for packet received, 0 for no packet received, or Error
 */
int32_t rfm95w_is_packet_received();

/**
 * @brief   Clear the last packet received flag.
 *
 * @param	      None
 * @return        0 for success, or Error
 */
int32_t rfm95w_clear_is_packet_received();

/**
 * @brief   Copy the last received LoRa Packet into the buffer.
 *
 * @param[in]	  max_buffer_length	length of the buffer to copy into.
 * @param[out]	  buffer buffer to copy into.
 * @param[out]	  received_buffer_length size of packet copied into the buffer
 * @return        0 for success or Error
 */
int32_t rfm95w_get_received_packet(uint8_t max_buffer_length, volatile uint8_t buffer[max_buffer_length], volatile uint8_t* received_buffer_length);



/**
 * @brief   Process Interrupts from RFM95W module.
 *
 * @param         None
 * @return        0 for success or Error
 */
int32_t rfm95w_process_interrupt();



#endif /* RFM95W_H */

/* End of file */
