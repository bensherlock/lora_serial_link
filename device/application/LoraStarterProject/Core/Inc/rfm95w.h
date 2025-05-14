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

/*
 * Public: Constants and Macros 
 */

#define RFM95W_EN_GPIO_PIN 		GPIO_PIN_0		/*!< EN pin */
#define RFM95W_EN_GPIO_PORT 	GPIOA			/*!< EN port */
#define RFM95W_G0_GPIO_PIN 		GPIO_PIN_2		/*!< G0 pin */
#define RFM95W_G0_GPIO_PORT 	GPIOA			/*!< G0 port */
#define RFM95W_RST_GPIO_PIN 	GPIO_PIN_3		/*!< RST pin */
#define RFM95W_RST_GPIO_PORT 	GPIOA			/*!< RST port */


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



#endif /* RFM95W_H */

/* End of file */
