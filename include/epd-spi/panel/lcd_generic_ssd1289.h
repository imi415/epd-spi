#ifndef LCD_GENERIC_SSD1289_H
#define LCD_GENERIC_SSD1289_H

#include "epd-spi/epd_common.h"

/*
 * Three 262k Color Modes:
 * A: RG/
 */

typedef enum {
    LCD_GENERIC_SSD1289_MODE_RGB888   = 0,
    LCD_GENERIC_SSD1289_MODE_XBRG8888 = 1,
    LCD_GENERIC_SSD1289_MODE_GRXB8888 = 2,
    LCD_GENERIC_SSD1289_MODE_RGB565   = 3,
} lcd_generic_ssd1289_mode_t;

typedef enum {
    LCD_GENERIC_SSD1289_DIR_HORIZONTAL = 0,
    LCD_GENERIC_SSD1289_DIR_VERTICAL   = 1,
} lcd_generic_ssd1289_dir_t;

typedef struct {
    lcd_generic_ssd1289_mode_t mode;
    lcd_generic_ssd1289_dir_t  dir;
    epd_cb_t                   cb;
    void                      *user_data;
} lcd_generic_ssd1289_t;

epd_ret_t lcd_generic_ssd1289_init(lcd_generic_ssd1289_t *lcd);
epd_ret_t lcd_generic_ssd1289_upload(lcd_generic_ssd1289_t *lcd, epd_coord_t *coord, uint8_t *data);

#endif