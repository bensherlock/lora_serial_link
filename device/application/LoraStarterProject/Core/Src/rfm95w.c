/**
 * @file    rfm95w.c
 *
 * @brief   LoRa RFM95W Module.
 *
 * @copyright Copyright (c) 2025 Ben Sherlock
 *
 *  <multi-line description>
 *
 */


/*
 * Includes
 */
#include "rfm95w.h"



/*
 * Private: Constants and Macros 
 */

//#define U	(1)		/*!<  */

/*
 * Register Names (LoRa Mode) SX1276 Datasheet 4.1, Table 41
 */
#define RFM95W_REG_00_FIFO                              0x00
#define RFM95W_REG_01_OP_MODE                           0x01
#define RFM95W_REG_02_RESERVED                       	0x02
#define RFM95W_REG_03_RESERVED                          0x03
#define RFM95W_REG_04_RESERVED                          0x04
#define RFM95W_REG_05_RESERVED                          0x05
#define RFM95W_REG_06_FRF_MSB                           0x06
#define RFM95W_REG_07_FRF_MID                           0x07
#define RFM95W_REG_08_FRF_LSB                           0x08
#define RFM95W_REG_09_PA_CONFIG                         0x09
#define RFM95W_REG_0A_PA_RAMP                           0x0a
#define RFM95W_REG_0B_OCP                               0x0b
#define RFM95W_REG_0C_LNA                               0x0c
#define RFM95W_REG_0D_FIFO_ADDR_PTR                     0x0d
#define RFM95W_REG_0E_FIFO_TX_BASE_ADDR                 0x0e
#define RFM95W_REG_0F_FIFO_RX_BASE_ADDR                 0x0f
//
#define RFM95W_REG_10_FIFO_RX_CURRENT_ADDR              0x10
#define RFM95W_REG_11_IRQ_FLAGS_MASK                    0x11
#define RFM95W_REG_12_IRQ_FLAGS                         0x12
#define RFM95W_REG_13_RX_NB_BYTES                       0x13
#define RFM95W_REG_14_RX_HEADER_CNT_VALUE_MSB           0x14
#define RFM95W_REG_15_RX_HEADER_CNT_VALUE_LSB           0x15
#define RFM95W_REG_16_RX_PACKET_CNT_VALUE_MSB           0x16
#define RFM95W_REG_17_RX_PACKET_CNT_VALUE_LSB           0x17
#define RFM95W_REG_18_MODEM_STAT                        0x18
#define RFM95W_REG_19_PKT_SNR_VALUE                     0x19
#define RFM95W_REG_1A_PKT_RSSI_VALUE                    0x1a
#define RFM95W_REG_1B_RSSI_VALUE                        0x1b
#define RFM95W_REG_1C_HOP_CHANNEL                       0x1c
#define RFM95W_REG_1D_MODEM_CONFIG1                     0x1d
#define RFM95W_REG_1E_MODEM_CONFIG2                     0x1e
#define RFM95W_REG_1F_SYMB_TIMEOUT_LSB                  0x1f
//
#define RFM95W_REG_20_PREAMBLE_MSB                      0x20
#define RFM95W_REG_21_PREAMBLE_LSB                      0x21
#define RFM95W_REG_22_PAYLOAD_LENGTH                    0x22
#define RFM95W_REG_23_MAX_PAYLOAD_LENGTH                0x23
#define RFM95W_REG_24_HOP_PERIOD                        0x24
#define RFM95W_REG_25_FIFO_RX_BYTE_ADDR                 0x25
#define RFM95W_REG_26_MODEM_CONFIG3                     0x26
#define RFM95W_REG_27_RESERVED                          0x27
#define RFM95W_REG_28_FEI_MSB                           0x28
#define RFM95W_REG_29_FEI_MID                           0x29
#define RFM95W_REG_2A_FEI_LSB                           0x2a
#define RFM95W_REG_2B_RESERVED                          0x2b
#define RFM95W_REG_2C_RSSI_WIDEBAND                     0x2c
#define RFM95W_REG_2D_RESERVED                       	0x2d
#define RFM95W_REG_2E_RESERVED                   	    0x2e
#define RFM95W_REG_2F_IF_FREQ1                    	   	0x2f
//
#define RFM95W_REG_30_IF_FREQ2                    	  	0x30
#define RFM95W_REG_31_DETECT_OPTIMIZE                   0x31
#define RFM95W_REG_32_RESERVED                     		0x32
#define RFM95W_REG_33_INVERT_IQ                         0x33
#define RFM95W_REG_34_RESERVED                     		0x34
#define RFM95W_REG_35_RESERVED                     		0x35
#define RFM95W_REG_36_HIGH_BW_OPTIMIZE1           		0x36
#define RFM95W_REG_37_DETECTION_THRESHOLD               0x37
#define RFM95W_REG_38_RESERVED                     		0x38
#define RFM95W_REG_39_SYNC_WORD                         0x39
#define RFM95W_REG_3A_HIGH_BW_OPTIMIZE2           		0x3a
#define RFM95W_REG_3B_INVERT_IQ2           				0x3b
#define RFM95W_REG_3C_RESERVED                     		0x3c
#define RFM95W_REG_3D_RESERVED                     		0x3d
#define RFM95W_REG_3E_RESERVED                     		0x3e
#define RFM95W_REG_3F_RESERVED                     		0x3f
//
#define RFM95W_REG_40_DIO_MAPPING1                      0x40
#define RFM95W_REG_41_DIO_MAPPING2                      0x41
#define RFM95W_REG_42_VERSION                           0x42
#define RFM95W_REG_44_RESERVED                     		0x44
#define RFM95W_REG_4B_TCXO                              0x4b
#define RFM95W_REG_4D_PA_DAC                            0x4d
//
#define RFM95W_REG_5B_FORMER_TEMP                       0x5b
#define RFM95W_REG_5D_RESERVED                       	0x5d
//
#define RFM95W_REG_61_AGC_REF                           0x61
#define RFM95W_REG_62_AGC_THRESH1                       0x62
#define RFM95W_REG_63_AGC_THRESH2                       0x63
#define RFM95W_REG_64_AGC_THRESH3                       0x64
//
#define RFM95W_REG_70_PLL                       		0x70


/*
 * Register Values (LoRa Mode) SX1276 Datasheet 4.4, Table
 */

// RFM95W_REG_01_OP_MODE
#define RFM95W_REGVAL_01_FSKOOK_MODE              		0x00	/*!< bits 7 */
#define RFM95W_REGVAL_01_LONG_RANGE_MODE              	0x80	/*!< bits 7 */

#define RFM95W_REGVAL_01_ACCESS_SHARED_REG             	0x40	/*!< bits 6 */

#define RFM95W_REGVAL_01_HIGH_FREQUENCY_MODE            0x00	/*!< bits 3 */
#define RFM95W_REGVAL_01_LOW_FREQUENCY_MODE            	0x08	/*!< bits 3 */

#define RFM95W_REGVAL_01_MODE_SLEEP            			0x00	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_STDBY            			0x01	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_FSTX            			0x02	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_TX            			0x03	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_FSRX            			0x04	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_RXCONTINUOUS            	0x05	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_RXSINGLE            		0x06	/*!< bits 2-0 */
#define RFM95W_REGVAL_01_MODE_CAD            			0x07	/*!< bits 2-0 */



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

static SPI_HandleTypeDef* g_spi_handle = 0;		/*!< SPI Handle */

/*
 * Private: Function Prototypes/Declarations
 */


/*
 * Public: Function Definitions
 */

/**
 * @brief   Initialise RFM95W module.
 *
 * @param[in]     spi_handle
 * @return        0 for success or Error
 */
int32_t rfm95w_init(SPI_HandleTypeDef* spi_handle)
{

	g_spi_handle = spi_handle;


	// Initialise the external RFM95W module.


    g_initialised = 1;

    return (0);
}




/*
 * Private: Function Definitions
 */




/* End of file */
