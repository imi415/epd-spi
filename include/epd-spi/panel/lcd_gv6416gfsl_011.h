#ifndef LCD_PANEL_GV6416GFSL_011_H
#define LCD_PANEL_GV6416GFSL_011_H

#include "epd-spi/epd_common.h"

typedef struct {
    epd_cb_t cb;
    void *user_data;
} lcd_gv6416g_t;

epd_ret_t lcd_gv6416g_init(lcd_gv6416g_t *lcd);
epd_ret_t lcd_gv6416g_upload(lcd_gv6416g_t *lcd, epd_coord_t *coord, uint8_t *data);

#endif