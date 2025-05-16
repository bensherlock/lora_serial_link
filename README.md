# LoRa Serial Link
Transparent Wireless Serial Link using LoRa.


## Hardware Modules

### Microcontroller 
ARM Cortex M4 STM32L496 [https://www.st.com/en/microcontrollers-microprocessors/stm32l4x6/documentation.html](https://www.st.com/en/microcontrollers-microprocessors/stm32l4x6/documentation.html)
 [https://www.st.com/resource/en/datasheet/stm32l496ae.pdf](https://www.st.com/resource/en/datasheet/stm32l496ae.pdf)

### RS232 Driver 
MAX3381E [https://www.analog.com/en/products/max3381e.html](https://www.analog.com/en/products/max3381e.html)

### LoRa Module 
Adafruit RFM95W LoRa [https://www.adafruit.com/product/3072](https://www.adafruit.com/product/3072) 
breakout board for [https://www.nicerf.com/lora-module/long-range-lora-module-lora1276-c1.html](https://www.nicerf.com/lora-module/long-range-lora-module-lora1276-c1.html) 
containing SX1276 core [https://www.semtech.com/products/wireless-rf/lora-connect/sx1276](https://www.semtech.com/products/wireless-rf/lora-connect/sx1276)
extra application notes for the SX127x family [https://www.semtech.com/products/wireless-rf/lora-connect/sx1272](https://www.semtech.com/products/wireless-rf/lora-connect/sx1272)
Downloads: [https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/downloads](https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/downloads)


## Hardware Configuration

### Clocks
4MHz HSE
32.768kHz LSE

### Pin Configurations
+ LED (PC12)

Debug RS232 
+ LP_RS232_RXD => LPUART_RX (PB10)
+ LP_RS232_TXD => LPUART_TX (PB11)

Modem/Serial RS232
+ RS232_RXD => UART_RX (PA10)
+ RS232_TXD => UART_TX (PA9)

## Timers
TIM15 80Mhz Clock. /1000 prescaler. 4000-1 counter value. Result == 2Hz. 
Calling HAL_TIM_PeriodElapsedCallback to Toggle LED.

## UARTS
LPUART1 115200N8 baud.


 