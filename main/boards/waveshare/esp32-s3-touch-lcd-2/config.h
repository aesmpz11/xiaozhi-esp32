#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// --- System & Core Settings ---
#ifndef BOARD_NAME
#define BOARD_NAME "ESP32-S3-Touch-LCD-2"
#endif

// --- Audio Configuration (I2S Mic & Amplifier) ---
#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_OUTPUT_SAMPLE_RATE 16000
#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_NC
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_2   // Word Select / LRC
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_4   // Bit Clock / SCK
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_10  // Microphone Data In (SD)
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_8   // Speaker Amplifier Data Out (DIN)

// --- Display Configuration (ST7789 2-inch SPI Display) ---
#define DISPLAY_WIDTH       240
#define DISPLAY_HEIGHT      320
#define DISPLAY_SPI_HOST    SPI2_HOST
#define DISPLAY_MOSI_PIN    GPIO_NUM_38
#define DISPLAY_SCLK_PIN    GPIO_NUM_39
#define DISPLAY_CS_PIN      GPIO_NUM_45
#define DISPLAY_DC_PIN      GPIO_NUM_42
#define DISPLAY_BL_PIN      GPIO_NUM_1
#define DISPLAY_BACKLIGHT_PIN DISPLAY_BL_PIN
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false
#define DISPLAY_RST_PIN     GPIO_NUM_NC

// --- Touch Panel Configuration (CST816D Capacitive Touch) ---
#define TOUCH_I2C_PORT      0
#define TOUCH_SDA_PIN       GPIO_NUM_48
#define TOUCH_SCL_PIN       GPIO_NUM_47
#define TOUCH_RST_PIN       GPIO_NUM_NC
#define TOUCH_INT_PIN       GPIO_NUM_16

// --- System Buttons ---
#define BOOT_BUTTON_PIN     GPIO_NUM_0

#endif