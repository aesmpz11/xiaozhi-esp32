This PR introduces full board initialization and hardware integration for the Waveshare ESP32-S3-Touch-LCD-2 within the Xiaozhi firmware framework.
Hardware Components Integrated:

    Display: ST7789 display driver configured with correct resolution and color inversion.

    Touch Interface: CST816S touch controller over I2C.

    Audio & Core: Configured via the WifiBoard class leveraging esp_lvgl_port and LVGL dependencies.

Tested successfully on physical hardware.



GPIO pinnings for the hardware are as follows (please note that pins 2 and 4 are shared by both the amp and the mic):

BCLK (Bit Clock / SCK): GPIO 4

WS / LRC (Word Select): GPIO 2

DIN (Microphone SD): GPIO 10

DOUT (Amplifier DIN): GPIO 8 





