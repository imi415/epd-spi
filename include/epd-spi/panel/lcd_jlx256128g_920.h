#ifndef LCD_PANEL_JLX256128G_920_H
#define LCD_PANEL_JLX256128G_920_H

#include "epd-spi/epd_common.h"

/**
 * @brief JLX256128G-920 LCD
 * Drive/Controller: Sitronix ST75256
 * Resolution: 256x128@2bpp(4 grayscale)
 * Link: https://item.taobao.com/item.htm?id=595485128701
 * Datasheet: https://pan.baidu.com/s/1XVAYUiLdCP7uPqK4mrkUxQ Pass: e87n
 */

typedef enum {
    LCD_JLX256128G_MODE_BW = 0x10U,
    LCD_JLX256128G_MODE_GS = 0x11U,
} lcd_jlx256128g_mode_t;

typedef struct {
    lcd_jlx256128g_mode_t mode;
    uint16_t               contrast;
} lcd_jlx256128g_config_t;

typedef struct {
    epd_cb_t                cb;
    void                   *user_data;
    lcd_jlx256128g_config_t config;
} lcd_jlx256128g_t;

epd_ret_t lcd_jlx256128g_init(lcd_jlx256128g_t *lcd);
epd_ret_t lcd_jlx256128g_power(lcd_jlx256128g_t *lcd, uint8_t on);
epd_ret_t lcd_jlx256128g_upload(lcd_jlx256128g_t *lcd, epd_coord_t *coord, uint8_t *data);

#endif