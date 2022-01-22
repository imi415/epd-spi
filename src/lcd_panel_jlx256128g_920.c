#include "lcd_panel_jlx256128g_920.h"

#include "epd_private.h"

#define PANEL_X_OFFSET 0
#define PANEL_Y_OFFSET 32

static uint8_t lcd_jlx256128g_init_sequence[] = {
    0x00, 0x30,                    // Extension command EXT[1:0] = 0,0
    0x00, 0x94,                    // Set power save mode, SLP = 0
    0x00, 0x31,                    // Extension command EXT[1:0] = 0,1
    0x01, 0xD7, 0x9F,              // Set auto-read instruction, XARD = 1
    0x03, 0x32, 0x00, 0x01, 0x03,  // 32 - Set analog circuit, BE[1:0] = 0,1; BS[2:0] = 0,1,1
    0x10, 0x20, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D,
    0x10, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,  // 31 - Set grey scale level
    0x04, 0xF0, 0x0D, 0x0D, 0x0D, 0x0D,                    // Frame rate, 0x0C - 69.0Hz
    0x00, 0x30,                                            // Extension command EXT[1:0] = 0,0
    0x02, 0x75, 0x00, 0x14,                                // 7 - Set page address, YS = 0x00, YE = 0x14
    0x02, 0x15, 0x00, 0xFF,                                // 8 - Set column address, XS = 0x00, XE = 0xFF
    0x01, 0xBC, 0x00,                                      // 9 - Data scan direction, MV = 0, MX = 0, MY = 0
    0x03, 0xCA, 0x00, 0x9F, 0x20,  // 5 - Display control, CLD = 0, DT = 0x7F, LF[4:0] = 16, FL = 0
    0x01, 0xF0, 0x10,              // 28 - Display mode: DM = 0(Mono)
    0x02, 0x81, 0x36, 0x04,        // 21 - Set VOP, VOP = 0x136
    0x01, 0x20, 0x0B,              // 20 - Power control, VB = 1, VF = 1, VR = 1
};

static epd_ret_t lcd_jlx256128g_reset(lcd_jlx256128g_t *lcd) {
    EPD_ERROR_CHECK(lcd->cb.reset_cb(lcd->user_data));

    return EPD_OK;
}

static epd_ret_t lcd_jlx256128g_window(lcd_jlx256128g_t *lcd, epd_coord_t *coord) {
    uint8_t pixel_per_byte = 4;
    if (lcd->config.mode == LCD_JLX256128G_MODE_BW) {
        pixel_per_byte = 8;
    }

    if (coord->y_start % pixel_per_byte != 0 || coord->y_end % pixel_per_byte != pixel_per_byte - 1) {
        return EPD_FAIL;
    }

    if (coord->x_end > 255 || coord->x_start > coord->x_end) {
        return EPD_FAIL;
    }

    if (coord->y_end > 127 || coord->y_start > coord->y_end) {
        return EPD_FAIL;
    }

    uint8_t page_start = (coord->y_start + PANEL_Y_OFFSET) / pixel_per_byte;
    uint8_t page_end   = (coord->y_end + PANEL_Y_OFFSET) / pixel_per_byte;

    uint8_t col_cmd[]  = {0x15, coord->x_start + PANEL_X_OFFSET, coord->x_end + PANEL_X_OFFSET};
    uint8_t page_cmd[] = {0x75, page_start, page_end};

    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, col_cmd, 0x03));
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, page_cmd, 0x03));

    return EPD_OK;
}

static epd_ret_t lcd_jlx256128g_config(lcd_jlx256128g_t *lcd) {
    uint8_t contrast_command[] = {0x81, lcd->config.contrast & 0x3F, (lcd->config.contrast >> 6U) & 0x07};
    uint8_t mode_command[]     = {0xF0, lcd->config.mode};

    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, contrast_command, 0x03));
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, mode_command, 0x02));

    epd_coord_t coord = {
        .x_start = 0,
        .x_end   = 255,
        .y_start = 0,
        .y_end   = 127,
    };

    EPD_ERROR_CHECK(lcd_jlx256128g_window(lcd, &coord));

    return EPD_OK;
}

static inline uint32_t lcd_jlx256128g_data_size(lcd_jlx256128g_t *lcd, epd_coord_t *coord) {
    uint8_t pix_per_byte = 8;
    if (lcd->config.mode == LCD_JLX256128G_MODE_GS) {
        pix_per_byte = 4;
    }

    return (coord->y_end - coord->y_start + 1) * (coord->x_end - coord->x_start + 1) / pix_per_byte;
}

epd_ret_t lcd_jlx256128g_init(lcd_jlx256128g_t *lcd) {
    EPD_ERROR_CHECK(lcd_jlx256128g_reset(lcd));
    EPD_ERROR_CHECK(epd_common_execute_sequence(&lcd->cb, lcd->user_data, lcd_jlx256128g_init_sequence,
                                                sizeof(lcd_jlx256128g_init_sequence)));
    EPD_ERROR_CHECK(lcd_jlx256128g_config(lcd));
    EPD_ERROR_CHECK(lcd_jlx256128g_power(lcd, 1));

    return EPD_OK;
}

epd_ret_t lcd_jlx256128g_power(lcd_jlx256128g_t *lcd, uint8_t on) {
    uint8_t tx_buf[] = {0xAF};
    if (!on) tx_buf[0] = 0xAE;
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, tx_buf, 0x01));

    return EPD_OK;
}

epd_ret_t lcd_jlx256128g_upload(lcd_jlx256128g_t *lcd, epd_coord_t *coord, uint8_t *data) {
    uint32_t data_size = (lcd->config.mode == LCD_JLX256128G_MODE_GS) ? 8192 : 4096;
    if (coord != NULL) {
        EPD_ERROR_CHECK(lcd_jlx256128g_window(lcd, coord));
        data_size = lcd_jlx256128g_data_size(lcd, coord);
    }

    uint8_t dtm_cmd[] = {0x5C};
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, dtm_cmd, 0x01));
    EPD_ERROR_CHECK(lcd->cb.write_data_cb(lcd->user_data, data, data_size));

    if (coord != NULL) {
        epd_coord_t new_coord = {
            .x_start = 0,
            .x_end   = 255,
            .y_start = 0,
            .y_end   = 127,
        };

        EPD_ERROR_CHECK(lcd_jlx256128g_window(lcd, &new_coord));
    }

    return EPD_OK;
}