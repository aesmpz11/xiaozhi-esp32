#include "wifi_board.h"

#include "codecs/no_audio_codec.h"

#include "display/lcd_display.h"

#include "esp_lcd_touch_cst816s.h"

#include "esp_lcd_panel_io.h"

#include "esp_lcd_panel_vendor.h"

#include "esp_lcd_panel_ops.h"

#include "application.h"

#include "button.h"

#include "config.h"

#include <driver/spi_master.h>

#include <driver/i2c_master.h>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>

#include <freertos/task.h>


#define TAG "Esp32S3TouchLcd2"


class Esp32S3TouchLcd2Board : public WifiBoard {

private:

    Button boot_button_;

    LcdDisplay* display_;

    esp_lcd_touch_handle_t touch_handle_;

    i2c_master_bus_handle_t i2c_bus_;


    void InitializeI2c() {

        i2c_master_bus_config_t i2c_bus_cfg = {

            .i2c_port = (i2c_port_t)TOUCH_I2C_PORT,

            .sda_io_num = TOUCH_SDA_PIN,

            .scl_io_num = TOUCH_SCL_PIN,

            .clk_source = I2C_CLK_SRC_DEFAULT,

            .glitch_ignore_cnt = 7,

            .intr_priority = 0,

            .trans_queue_depth = 0,

            .flags = {

                .enable_internal_pullup = true,

            },

        };

        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_));

    }


    void InitializeDisplayAndTouch() {

        // 1. Initialize SPI Bus for Display

        spi_bus_config_t buscfg = {};

        buscfg.mosi_io_num = DISPLAY_MOSI_PIN;

        buscfg.miso_io_num = -1;

        buscfg.sclk_io_num = DISPLAY_SCLK_PIN;

        buscfg.quadwp_io_num = -1;

        buscfg.quadhd_io_num = -1;

        buscfg.max_transfer_sz = DISPLAY_WIDTH * 80 * sizeof(uint16_t);

        ESP_ERROR_CHECK(spi_bus_initialize(DISPLAY_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));


        // 2. Install Panel IO

        esp_lcd_panel_io_handle_t panel_io = NULL;

        esp_lcd_panel_io_spi_config_t io_config = {};

        io_config.cs_gpio_num = DISPLAY_CS_PIN;

        io_config.dc_gpio_num = DISPLAY_DC_PIN;

        io_config.spi_mode = 0;

        io_config.pclk_hz = 40 * 1000 * 1000;

        io_config.trans_queue_depth = 10;

        io_config.lcd_cmd_bits = 8;

        io_config.lcd_param_bits = 8;

        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)DISPLAY_SPI_HOST, &io_config, &panel_io));


        // 3. Install ST7789 Display Panel Driver

        esp_lcd_panel_handle_t panel = NULL;

        esp_lcd_panel_dev_config_t panel_config = {};

        panel_config.reset_gpio_num = DISPLAY_RST_PIN;

        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;

        panel_config.bits_per_pixel = 16;

       

        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));

        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));

        ESP_ERROR_CHECK(esp_lcd_panel_init(panel));

       

        // Enable color inversion using official ESP-IDF API

        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, true));

        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));


        // 4. Initialize Framework SpiLcdDisplay Wrapper

        display_ = new SpiLcdDisplay(panel_io, panel, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, false, false, false);


        // 5. Initialize Touch Controller (CST816S) over I2C

        esp_lcd_panel_io_handle_t tp_io_handle = NULL;

        esp_lcd_panel_io_i2c_config_t tp_io_config = {};

        tp_io_config.dev_addr = 0x15;

        tp_io_config.scl_speed_hz = 400000;

        tp_io_config.control_phase_bytes = 1;

        tp_io_config.lcd_cmd_bits = 8;

        tp_io_config.lcd_param_bits = 0;

        tp_io_config.flags.dc_low_on_data = 0;

        tp_io_config.flags.disable_control_phase = 1;


        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus_, &tp_io_config, &tp_io_handle));


        esp_lcd_touch_config_t tp_cfg = {};

        tp_cfg.x_max = DISPLAY_WIDTH;

        tp_cfg.y_max = DISPLAY_HEIGHT;

        tp_cfg.rst_gpio_num = TOUCH_RST_PIN;

        tp_cfg.int_gpio_num = TOUCH_INT_PIN;

        tp_cfg.levels.reset = 0;

        tp_cfg.levels.interrupt = 0;

        tp_cfg.flags.swap_xy = 0;

        tp_cfg.flags.mirror_x = 0;

        tp_cfg.flags.mirror_y = 0;


        ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &touch_handle_));

    }


    void InitializeButtons() {

        boot_button_.OnClick([this]() {

            auto& app = Application::GetInstance();

            if (app.GetDeviceState() == kDeviceStateStarting) {

                EnterWifiConfigMode();

                return;

            }

            app.ToggleChatState();

        });

    }


public:

    Esp32S3TouchLcd2Board() : boot_button_(BOOT_BUTTON_PIN) {

        InitializeI2c();

        InitializeDisplayAndTouch();

        InitializeButtons();

        GetBacklight()->RestoreBrightness();

    }


    virtual AudioCodec* GetAudioCodec() override {

    static NoAudioCodecDuplex audio_codec(

        AUDIO_INPUT_SAMPLE_RATE,

        AUDIO_OUTPUT_SAMPLE_RATE,

        GPIO_NUM_4,   // BCLK / SCK (Shared)

        GPIO_NUM_2,   // WS / LRC (Shared)

        GPIO_NUM_8,   // DOUT (To Amplifier DIN)

        GPIO_NUM_10   // DIN (From Microphone SD)

    );

    return &audio_codec;

}


    virtual Display* GetDisplay() override {

        return display_;

    }


    virtual Backlight* GetBacklight() override {

        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);

        return &backlight;

    }

};


DECLARE_BOARD(Esp32S3TouchLcd2Board);