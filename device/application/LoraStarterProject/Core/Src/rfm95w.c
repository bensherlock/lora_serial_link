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
#include "dbg_output.h"

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_spi.h"

#include <stdint.h>


/*
 * Private: Constants and Macros 
 */

//#define U	(1)		/*!<  */

#define RFM95W_FXOSC	(32000000.0f)		/*!< 32MHz */

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

// RFM95W_REG_09_PA_CONFIG
#define RFM95W_REGVAL_09_PA_SELECT_BOOST				0x80	/*!< bits 7 */
//#define RFM95W_REGVAL_09_MAX_POWER					0x00	/*!< bits 6-4 */
//#define RFM95W_REGVAL_09_OUTPUT_POWER					0x00	/*!< bits 3-0 */

// RFM95W_REG_0A_PA_RAMP
#define RFM95W_REGVAL_0A_PA_RAMP_3_4MS					0x00	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_2MS					0x01	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_1MS					0x02	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_500US					0x03	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_250US					0x04	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_125US					0x05	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_100US					0x06	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_62US					0x07	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_50US					0x08	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_40US					0x09	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_31US					0x0a	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_25US					0x0b	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_20US					0x0c	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_15US					0x0d	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_12US					0x0e	/*!< bits 3-0 */
#define RFM95W_REGVAL_0A_PA_RAMP_10US					0x0f	/*!< bits 3-0 */

// RFM95W_REG_0B_OCP
#define RFM95W_REGVAL_0B_OCP_ON							0x20	/*!< bits 5 */
//#define RFM95W_REGVAL_0B_OCP_TRIM						0x00	/*!< bits 4-0 */


// RFM95W_REG_0C_LNA
#define RFM95W_REGVAL_0C_LNA_GAIN_G1					0x01	/*!< bits 7-5 */
#define RFM95W_REGVAL_0C_LNA_GAIN_G2					0x02	/*!< bits 7-5 */
#define RFM95W_REGVAL_0C_LNA_GAIN_G3					0x03	/*!< bits 7-5 */
#define RFM95W_REGVAL_0C_LNA_GAIN_G4					0x04	/*!< bits 7-5 */
#define RFM95W_REGVAL_0C_LNA_GAIN_G5					0x05	/*!< bits 7-5 */
#define RFM95W_REGVAL_0C_LNA_GAIN_G6					0x06	/*!< bits 7-5 */
#define RFM95W_REGVAL_0C_LNA_BOOST_HF_150PC             0x03	/*!< bits 1-0 */


// RFM95W_REG_11_IRQ_FLAGS_MASK
#define RFM95W_REGVAL_11_RX_TIMEOUT_MASK                0x80	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_RX_DONE_MASK                   0x40	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_PAYLOAD_CRC_ERROR_MASK         0x20	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_VALID_HEADER_MASK              0x10	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_TX_DONE_MASK                   0x08	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_CAD_DONE_MASK                  0x04	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_FHSS_CHANGE_CHANNEL_MASK       0x02	/*!< bits 7-0 */
#define RFM95W_REGVAL_11_CAD_DETECTED_MASK              0x01	/*!< bits 7-0 */


// RFM95W_REG_12_IRQ_FLAGS
#define RFM95W_REGVAL_12_RX_TIMEOUT                     0x80	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_RX_DONE                        0x40	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_PAYLOAD_CRC_ERROR              0x20	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_VALID_HEADER                   0x10	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_TX_DONE                        0x08	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_CAD_DONE                       0x04	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_FHSS_CHANGE_CHANNEL            0x02	/*!< bits 7-0 */
#define RFM95W_REGVAL_12_CAD_DETECTED                   0x01	/*!< bits 7-0 */



// RFM95W_REG_18_MODEM_STAT
//#define RFM95W_REGVAL_18_RX_CODING_RATE					0x00	/*!< bits 7-5 */
#define RFM95W_REGVAL_18_MODEM_STATUS_CLEAR					0x10	/*!< bits 4-0 */
#define RFM95W_REGVAL_18_MODEM_STATUS_HEADER_INFO_VALID		0x08	/*!< bits 4-0 */
#define RFM95W_REGVAL_18_MODEM_STATUS_RX_ONGOING			0x04	/*!< bits 4-0 */
#define RFM95W_REGVAL_18_MODEM_STATUS_SIGNAL_SYNCH			0x02	/*!< bits 4-0 */
#define RFM95W_REGVAL_18_MODEM_STATUS_SIGNAL_DETECT			0x01	/*!< bits 4-0 */


// RFM95W_REG_1D_MODEM_CONFIG1
#define RFM95W_REGVAL_1D_BW_7_8KHZ							0x00	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_10_4KHZ							0x10	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_15_6KHZ							0x20	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_20_8KHZ							0x30	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_31_25KHZ						0x40	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_41_7KHZ							0x50	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_62_5KHZ							0x60	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_125KHZ							0x70	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_250KHZ							0x80	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_BW_500KHZ							0x90	/*!< bits 7-4 */
#define RFM95W_REGVAL_1D_CODING_RATE_4_5					0x02	/*!< bits 3-1 */
#define RFM95W_REGVAL_1D_CODING_RATE_4_6					0x04	/*!< bits 3-1 */
#define RFM95W_REGVAL_1D_CODING_RATE_4_7					0x06	/*!< bits 3-1 */
#define RFM95W_REGVAL_1D_CODING_RATE_4_8					0x08	/*!< bits 3-1 */
#define RFM95W_REGVAL_1D_IMPLICIT_HEADER_MODE_ON			0x01	/*!< bits 0 */


// RFM95W_REG_1E_MODEM_CONFIG2
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_64_CHIPS			0x60	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_128_CHIPS			0x70	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_256_CHIPS			0x80	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_512_CHIPS			0x90	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_1024_CHIPS		0xa0	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_2048_CHIPS		0xb0	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_SPREADING_FACTOR_4096_CHIPS		0xc0	/*!< bits 7-4 */
#define RFM95W_REGVAL_1E_TX_CONTINUOUS_MODE					0x08	/*!< bits 3 */
#define RFM95W_REGVAL_1E_RX_PAYLOAD_CRC_ON					0x04	/*!< bits 2 */

// RFM95W_REG_26_MODEM_CONFIG3
#define RFM95W_REGVAL_26_LOW_DATA_RATE_OPTIMIZE				0x08	/*!< bits 3 */
#define RFM95W_REGVAL_26_AGC_AUTO_ON						0x04	/*!< bits 2 */

// RFM95W_REG_40_DIO_MAPPING1 - Table 18 DIO Mapping LoRa Mode
#define RFM95W_REGVAL_40_DIO0_RX_DONE						0x00	/*!< bits 7-6 */
#define RFM95W_REGVAL_40_DIO0_TX_DONE						0x40	/*!< bits 7-6 */
#define RFM95W_REGVAL_40_DIO0_CAD_DONE						0x80	/*!< bits 7-6 */
#define RFM95W_REGVAL_40_DIO1_RX_TIMEOUT					0x00	/*!< bits 5-4 */
#define RFM95W_REGVAL_40_DIO1_FHSS_CHANGE_CHANNEL			0x10	/*!< bits 5-4 */
#define RFM95W_REGVAL_40_DIO1_CAD_DETECTED					0x20	/*!< bits 5-4 */
#define RFM95W_REGVAL_40_DIO2_FHSS_CHANGE_CHANNEL			0x00	/*!< bits 3-2 */
#define RFM95W_REGVAL_40_DIO3_CAD_DONE						0x00	/*!< bits 1-0 */
#define RFM95W_REGVAL_40_DIO3_VALID_HEADER					0x01	/*!< bits 1-0 */
#define RFM95W_REGVAL_40_DIO3_PAYLOAD_CRC_ERROR				0x02	/*!< bits 1-0 */

// RFM95W_REG_41_DIO_MAPPING2
#define RFM95W_REGVAL_41_DIO4_CAD_DETECTED					0x00	/*!< bits 7-6 */
#define RFM95W_REGVAL_41_DIO4_PLL_LOCK						0x40	/*!< bits 7-6 */
#define RFM95W_REGVAL_41_DIO5_MODE_READY					0x00	/*!< bits 5-4 */
#define RFM95W_REGVAL_41_DIO5_CLK_OUT						0x40	/*!< bits 5-4 */


// RFM95W_REG_4D_PA_DAC
#define RFM95W_REGVAL_4D_PA_DAC_0_DBM						0x04	/*!< bits 2-0 */
#define RFM95W_REGVAL_4D_PA_DAC_1_DBM						0x05	/*!< bits 2-0 */
#define RFM95W_REGVAL_4D_PA_DAC_2_DBM						0x06	/*!< bits 2-0 */
#define RFM95W_REGVAL_4D_PA_DAC_3_DBM						0x07	/*!< bits 2-0 */



#define SPI_REGISTER_WRITE_FLAG			(0x80)

#define MAX_SPI_BUFFER_LENGTH	(255U)


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

static const uint8_t g_null_buffer[MAX_SPI_BUFFER_LENGTH] = {0U};	/*!< Buffer of zeros for transmission on SPI when we are only interested in receiving */

static const uint32_t g_receive_buffer_max_length = MAX_SPI_BUFFER_LENGTH;


/*
 * Private: Variables
 */
static uint8_t g_initialised = 0;           /*!< Initialised flag */

static SPI_HandleTypeDef* g_spi_handle = 0;		/*!< SPI Handle */

static volatile uint8_t g_regval = 0;

static volatile uint8_t g_receive_buffer[MAX_SPI_BUFFER_LENGTH] = {0};
static volatile uint8_t g_receive_buffer_length = 0;
static volatile uint8_t g_packet_received = 0;

/*
 * Private: Function Prototypes/Declarations
 */

/**
 * @brief   Write burst data to the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[in]     buffer_length the length of the data buffer to send.
 * @param[in]     buffer the data buffer to send.
 * @return        0 for success or Error
 */
static int32_t rfm95w_write_burst(const uint8_t register_address, const uint8_t buffer_length, const uint8_t buffer[buffer_length]);

/**
 * @brief   Write single data to the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[in]     data_byte the data byte to send.
 * @return        0 for success or Error
 */
static int32_t rfm95w_write_single(const uint8_t register_address, const uint8_t data_byte);

/**
 * @brief   Read burst data from the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[in]     buffer_length the length of the data buffer to receive.
 * @param[out]    buffer the data buffer to receive into.
 * @return        0 for success or Error
 */
static volatile int32_t rfm95w_read_burst(const uint8_t register_address, const uint8_t buffer_length, volatile uint8_t buffer[buffer_length]);

/**
 * @brief   Read single data from the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[out]    data_byte the data byte to receive into.
 * @return        0 for success or Error
 */
static volatile int32_t rfm95w_read_single(const uint8_t register_address, volatile uint8_t* data_byte);



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

	// Power Up the module if it isn't already
	HAL_Delay(200); // 200ms delay
	HAL_GPIO_WritePin(RFM95W_EN_GPIO_PORT, RFM95W_EN_GPIO_PIN, 1U);
	HAL_Delay(200); // 200ms delay


	// Hard Reset
	HAL_Delay(200); // 200ms delay
	HAL_GPIO_WritePin(RFM95W_RST_GPIO_PORT, RFM95W_RST_GPIO_PIN, 0U);
	HAL_Delay(200); // 200ms delay
	HAL_GPIO_WritePin(RFM95W_RST_GPIO_PORT, RFM95W_RST_GPIO_PIN, 1U);
	HAL_Delay(200); // 200ms delay

	// Set Sleep Mode to allow us to change to LoRa mode.
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, (RFM95W_REGVAL_01_LONG_RANGE_MODE | RFM95W_REGVAL_01_MODE_SLEEP));


	// Debug read back the register value
	//rfm95w_read_single(RFM95W_REG_01_OP_MODE, &g_regval);
	//asm("nop");

	// Setup the FIFO to use the entire 256 bytes for transmit and receive packets
	rfm95w_write_single(RFM95W_REG_0E_FIFO_TX_BASE_ADDR, 0);
	rfm95w_write_single(RFM95W_REG_0F_FIFO_RX_BASE_ADDR, 0);

	// Set to standby mode
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_STDBY);

	// Set the modem configuration - Modem PHY config 1,2,3.
	rfm95w_write_single(RFM95W_REG_1D_MODEM_CONFIG1, (RFM95W_REGVAL_1D_BW_125KHZ | RFM95W_REGVAL_1D_CODING_RATE_4_5));
	rfm95w_write_single(RFM95W_REG_1E_MODEM_CONFIG2, (RFM95W_REGVAL_1E_SPREADING_FACTOR_128_CHIPS | RFM95W_REGVAL_1E_RX_PAYLOAD_CRC_ON));
	rfm95w_write_single(RFM95W_REG_26_MODEM_CONFIG3, (RFM95W_REGVAL_26_AGC_AUTO_ON));

	// https://www.thethingsnetwork.org/docs/lorawan/lora-phy-format/
	// Preamble is used to synchronize the receiver with the transmitter.
	// It MUST consist of 8 symbols for all regions as mentioned in the LoRaWAN Regional Parameters document.
	// However, the radio transmitter will add another 4.25 symbols resulting in a final preamble length of 8 + 4.25 = 12.25 symbols.
	// Set the preamble length = length + 4.25 symbols
	uint16_t preamble_length = 8;
	uint8_t preamble_length_msb = (uint8_t)(preamble_length >> 8U);
	uint8_t preamble_length_lsb =  (uint8_t)(preamble_length & 0xFF);
	rfm95w_write_single(RFM95W_REG_20_PREAMBLE_MSB, preamble_length_msb);
	rfm95w_write_single(RFM95W_REG_21_PREAMBLE_LSB, preamble_length_lsb);

	// Set the frequency to 868MHz
	//RFM95W_FREQ_RF 868MHz
	//RFM95W_FXOSC 32MHz
	// Frf = FREQ_RF * 2^19 / FXOSC

	uint32_t frf = (uint32_t)((RFM95W_FREQ_RF * 524288.0f) / RFM95W_FXOSC);
	uint8_t frf_msb = (uint8_t)((frf >> 16U) & 0xFF);
	uint8_t frf_mid = (uint8_t)((frf >> 8U) & 0xFF);
	uint8_t frf_lsb =  (uint8_t)(frf & 0xFF);
	rfm95w_write_single(RFM95W_REG_06_FRF_MSB, frf_msb);
	rfm95w_write_single(RFM95W_REG_07_FRF_MID, frf_mid);
	rfm95w_write_single(RFM95W_REG_08_FRF_LSB, frf_lsb);

	// Set the tx power - page 83.
	// -4 dBm to +15 dBm from PA_HF/PA_LF.
	// +2 dBm to +17 dBm from PA_HP/PA_BOOST.
	// PA_DAC enables maximum power up to +20dBm.
	// Operation up to +17dBm continuous, or +20dBm duty cycled operation.

	// PA_CONFIG:
	// - MaxPower. Pmax=10.8+0.6*MaxPower [dBm]
	// - OutputPower. RFO: Pout=Pmax-(15-OutputPower). PA_BOOST: Pout=17-(15-OutputPower).


	int8_t power_dbm = 10;
	if (power_dbm > 17)
	{
		// Enable the DAC mode - add an additional max +3dBm
		rfm95w_write_single(RFM95W_REG_4D_PA_DAC, RFM95W_REGVAL_4D_PA_DAC_3_DBM);
		rfm95w_write_single(RFM95W_REG_09_PA_CONFIG, RFM95W_REGVAL_09_PA_SELECT_BOOST | ((power_dbm-3)-2));
	}
	else
	{
		// Disable the DAC mode 0dBm
		rfm95w_write_single(RFM95W_REG_4D_PA_DAC, RFM95W_REGVAL_4D_PA_DAC_0_DBM);
		rfm95w_write_single(RFM95W_REG_09_PA_CONFIG, RFM95W_REGVAL_09_PA_SELECT_BOOST | (power_dbm-2));
	}

    g_initialised = 1;

    return (0);
}



/**
 * @brief   Transmit a LoRa Packet with the RFM95W module.
 *
 * @param[in]	  buffer_length	length of the buffer to transmit.
 * @param[in]	  buffer buffer to transmit.
 * @return        0 for success or Error
 */
int32_t rfm95w_transmit_packet(uint8_t buffer_length, uint8_t buffer[buffer_length])
{
	if (g_initialised == 0)
	{
		// Error
		return -1;
	}

	// Explicit Mode:
	// Preamble (8 symbols)
	// PHDR (Physical Header) - Information about Payload Size and CRC Coding Rate.
	// PHDR_CRC (Header CRC) - CRC to correct errors in header
	//   PHDR and PHDR_CRC are encoded with fixed Coding Rate of 4/8
	// PHYPayload - Maximum size varies by data rate - region specific.
	// CRC - CRC for payload (uplink packet only)
	//   PHYPayload and CRC are encoded with Coding Rate options of 4/5, 4/6, 4/7, or 4/8.

	// Implicit Mode:
	// Preamble (8 symbols)
	// BCNPayload - Beacon Payload - used for time synchronisation from gateways to end devices.

	// Set to standby
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_STDBY);

	// Set the FIFO to address zero
	rfm95w_write_single(RFM95W_REG_0D_FIFO_ADDR_PTR, 0);

	// Write the LoRa payload
	rfm95w_write_burst(RFM95W_REG_00_FIFO, buffer_length, &buffer[0]);

	// Write the length
	rfm95w_write_single(RFM95W_REG_22_PAYLOAD_LENGTH, buffer_length);

	// Now transmit
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_TX);

	uint8_t irq_flags;
	rfm95w_read_single(RFM95W_REG_12_IRQ_FLAGS, &irq_flags);
	while ((irq_flags & RFM95W_REGVAL_12_TX_DONE) != RFM95W_REGVAL_12_TX_DONE)
	{
		rfm95w_read_single(RFM95W_REG_12_IRQ_FLAGS, &irq_flags);
	}

	// Transmission Complete
	// Clear IRQ Flags
	rfm95w_write_single(RFM95W_REG_12_IRQ_FLAGS, 0xFF);

	// Back into Standby
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_STDBY);

	return (0);
}


/**
 * @brief   Listen for incoming LoRa Packets with the RFM95W module.
 *
 * @param	      None
 * @return        0 for success or Error
 */
int32_t rfm95w_listen_for_packets()
{
	if (g_initialised == 0)
	{
		// Error
		return -1;
	}

	// Back into Standby
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_STDBY);

	// Read the IRQ flags
	uint8_t irq_flags;
	rfm95w_read_single(RFM95W_REG_12_IRQ_FLAGS, &irq_flags);

	rfm95w_write_single(RFM95W_REG_12_IRQ_FLAGS, 0xFF); // Clear IRQ flags


	// Rx Continuous Mode
	rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_RXCONTINUOUS);

	// Set interrupt for DIO0 on Rx Done
	rfm95w_write_single(RFM95W_REG_40_DIO_MAPPING1, RFM95W_REGVAL_40_DIO0_RX_DONE);

	// Enable Interrupt on GPIO


	return (0);
}


/**
 * @brief   Receive a LoRa Packet with the RFM95W module.
 *
 * @param[in]	  max_buffer_length	length of the buffer to receive into.
 * @param[out]	  buffer buffer to receive into.
 * @param[out]	  received_buffer_length count of bytes received.
 * @return        0 for success or Error
 */
int32_t rfm95w_receive_packet(uint8_t max_buffer_length, volatile uint8_t buffer[max_buffer_length], volatile uint8_t* received_buffer_length)
{
	if (g_initialised == 0)
	{
		// Error
		return -1;
	}

	uint8_t irq_flags;
	rfm95w_read_single(RFM95W_REG_12_IRQ_FLAGS, &irq_flags);

	// Check for Rx Timeout or CRC error
	if (irq_flags & (RFM95W_REGVAL_12_RX_TIMEOUT | RFM95W_REGVAL_12_PAYLOAD_CRC_ERROR))
	{
		// Back into Standby
		rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_STDBY);

		*received_buffer_length = 0;
	}
	else if (irq_flags & (RFM95W_REGVAL_12_RX_DONE | RFM95W_REGVAL_12_VALID_HEADER))
	{
		// Read the payload length
		rfm95w_read_single(RFM95W_REG_13_RX_NB_BYTES, received_buffer_length);

		// Read the start address of the current rx packet
		uint8_t rx_current_address;
		rfm95w_read_single(RFM95W_REG_10_FIFO_RX_CURRENT_ADDR, &rx_current_address);

		// Set the fifo pointer address for this packet
		rfm95w_write_single(RFM95W_REG_0D_FIFO_ADDR_PTR, rx_current_address);

		// Read the lora payload into the buffer
		rfm95w_read_burst(RFM95W_REG_00_FIFO, *received_buffer_length, &buffer[0]);

		// Read SNR and RSSI values of the last packet
		uint8_t snr;
		uint8_t rssi;
		rfm95w_read_single(RFM95W_REG_19_PKT_SNR_VALUE, &snr);
		rfm95w_read_single(RFM95W_REG_1A_PKT_RSSI_VALUE, &rssi);

		// Back into Standby
		rfm95w_write_single(RFM95W_REG_01_OP_MODE, RFM95W_REGVAL_01_MODE_STDBY);
	}
	else
	{
		*received_buffer_length = 0;
	}


	return (0);
}


/**
 * @brief   Process Interrupts from RFM95W module.
 *
 * @param         None
 * @return        0 for success or Error
 */
int32_t rfm95w_process_interrupt()
{
	if (g_initialised == 0)
	{
		// Error
		return -1;
	}

	rfm95w_receive_packet(g_receive_buffer_max_length, g_receive_buffer, &g_receive_buffer_length);
	if (g_receive_buffer_length > 0)
	{
		g_packet_received = 1;

		// debug
		dbg_output_write_str("Packet:");
		//dbg_output_write_buffer(g_receive_buffer_length, &g_receive_buffer[0]);
		// Or output as hex encoded values
		dbg_output_write_hex_encoded_csv_buffer(g_receive_buffer_length, &g_receive_buffer[0]);
		dbg_output_write_str("\r\n");
	}

	 // start listening
	 rfm95w_listen_for_packets();


	return (0);
}

/*
 * Private: Function Definitions
 */


/**
 * @brief   Write burst data to the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[in]     buffer_length the length of the data buffer to send.
 * @param[in]     buffer the data buffer to send.
 * @return        0 for success or Error
 */
static int32_t rfm95w_write_burst(const uint8_t register_address, const uint8_t buffer_length, const uint8_t buffer[buffer_length])
{
	uint8_t tx_byte = register_address | SPI_REGISTER_WRITE_FLAG; // set the write flag

	// Chip Select low at start of frame
	HAL_GPIO_WritePin(RFM95W_CS_GPIO_PORT, RFM95W_CS_GPIO_PIN, 0U);
	// Transmit the address and write flag
	HAL_SPI_Transmit(g_spi_handle, &tx_byte, 1, 100);
	// transmit the remaining data
	HAL_SPI_Transmit(g_spi_handle, &buffer[0], buffer_length, 100);
	// Chip Select high at end of frame
	HAL_GPIO_WritePin(RFM95W_CS_GPIO_PORT, RFM95W_CS_GPIO_PIN, 1U);

	asm("nop");

	return 0;
}


/**
 * @brief   Write single data to the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[in]     data_byte the data byte to send.
 * @return        0 for success or Error
 */
static int32_t rfm95w_write_single(const uint8_t register_address, const uint8_t data_byte)
{
	return rfm95w_write_burst(register_address, 1, &data_byte);
}


/**
 * @brief   Read burst data from the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[in]     buffer_length the length of the data buffer to receive.
 * @param[out]    buffer the data buffer to receive into.
 * @return        0 for success or Error
 */
static volatile int32_t rfm95w_read_burst(const uint8_t register_address, const uint8_t buffer_length, volatile uint8_t buffer[buffer_length])
{
	uint8_t tx_byte = register_address & ~(SPI_REGISTER_WRITE_FLAG); // clear the write flag

	// Chip Select low at start of frame
	HAL_GPIO_WritePin(RFM95W_CS_GPIO_PORT, RFM95W_CS_GPIO_PIN, 0U);
	// Transmit the address and write flag
	HAL_SPI_Transmit(g_spi_handle, &tx_byte, 1, 100);
	// receive the remaining data
	HAL_SPI_TransmitReceive(g_spi_handle, &g_null_buffer[0], &buffer[0], buffer_length, 100);
	// Chip Select high at end of frame
	HAL_GPIO_WritePin(RFM95W_CS_GPIO_PORT, RFM95W_CS_GPIO_PIN, 1U);

	asm("nop");

	return 0;
}


/**
 * @brief   Read single data from the RFM95W module.
 *
 * @param[in]     register_address the register to begin writing to.
 * @param[out]    data_byte the data byte to receive into.
 * @return        0 for success or Error
 */
static volatile int32_t rfm95w_read_single(const uint8_t register_address, volatile uint8_t* data_byte)
{
	return rfm95w_read_burst(register_address, 1, data_byte);
}

/* End of file */
