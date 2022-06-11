#include "panel/lcd_generic_ssd1289.h"

#include "epd_private.h"

/* Note: The command params is BE encoded, byte-reversing is required when sent from 16bit LE bus. */
static uint8_t lcd_generic_ssd1289_init_sequence[] = {
    0x02, 0x00, 0x00, 0x01, /* R00h - Oscillator Enable */
    0x02, 0x03, 0xA8, 0xA4, /* R03h - Power Control (1) */
    0x02, 0x0C, 0x00, 0x00, /* R0Ch - Power Control (2) */
    0x02, 0x0D, 0x08, 0x0C, /* R0Dh - Power Control (3) */
    0x02, 0x0E, 0x2B, 0x00, /* R0Eh - Power Control (4) */
    0x02, 0x1E, 0x00, 0xB0, /* R1Eh - Power Control (5) */
    0x02, 0x01, 0x2B, 0x3F, /* R01h - Driver Output Control */
    0x02, 0x02, 0x06, 0x00, /* R02h - LCD Driver AC Control */
    0x02, 0x10, 0x00, 0x00, /* R10h - Sleep Mode */
    0x02, 0x11, 0x60, 0x70, /* R11h - Entry Mode */
    0x02, 0x05, 0x00, 0x00, /* R05h - Compare Register (1) */
    0x02, 0x06, 0x00, 0x00, /* R06h - Compare Register (2) */
    0x02, 0x16, 0xEF, 0x1C, /* R16h - Horizontal Porch */
    0x02, 0x17, 0x00, 0x03, /* R17h - Vertical Porch */
    0x02, 0x07, 0x02, 0x33, /* R07h - Display Control */
    0x02, 0x0B, 0x00, 0x00, /* R0Bh - Frame Cycle Control */
    0x02, 0x0F, 0x00, 0x00, /* R0Fh - Gate Scan Start Position */
    0x02, 0x41, 0x00, 0x00, /* R41h - Vertical Scroll Control (1) */
    0x02, 0x42, 0x00, 0x00, /* R42h - Vertical Scroll Control (2) */
    0x02, 0x48, 0x00, 0x00, /* R48h - First Window Start */
    0x02, 0x49, 0x01, 0x3F, /* R49h - First Window End */
    0x02, 0x4A, 0x00, 0x00, /* R4Ah - Second Window Start */
    0x02, 0x4B, 0x00, 0x00, /* R4Bh - Second Window End */
    0x02, 0x44, 0xEF, 0x00, /* R44h - Horizontal RAM Address Position */
    0x02, 0x45, 0x00, 0x00, /* R45h - Vertical RAM Address Start Position */
    0x02, 0x46, 0x01, 0x3F, /* R46h - Vertical RAM Address End Position */
    0x02, 0x30, 0x07, 0x07, /* R30h - Gamma Control ( 1) */
    0x02, 0x31, 0x02, 0x04, /* R31h - Gamma Control ( 2) */
    0x02, 0x32, 0x02, 0x04, /* R32h - Gamma Control ( 3) */
    0x02, 0x33, 0x05, 0x02, /* R33h - Gamma Control ( 4) */
    0x02, 0x34, 0x05, 0x07, /* R34h - Gamma Control ( 5) */
    0x02, 0x35, 0x02, 0x04, /* R35h - Gamma Control ( 6) */
    0x02, 0x36, 0x02, 0x04, /* R36h - Gamma Control ( 7) */
    0x02, 0x37, 0x05, 0x02, /* R37h - Gamma Control ( 8) */
    0x02, 0x3A, 0x03, 0x02, /* R3Ah - Gamma Control ( 9) */
    0x02, 0x3B, 0x03, 0x02, /* R3Bh - Gamma Control (10) */
    0x02, 0x23, 0x00, 0x00, /* R23h - RAM Write Data Mask (1) */
    0x02, 0x24, 0x00, 0x00, /* R24h - RAM Write Data Mask (2) */
    0x02, 0x25, 0x80, 0x00, /* R25h - Frame Frequency */
    0x02, 0x4E, 0x00, 0x00, /* R4Eh - Set GDDRAM X Address Counter */
    0x02, 0x4F, 0x00, 0x00, /* R4Fh - Set GDDRAM Y Address Counter */
};

static inline epd_ret_t lcd_generic_ssd1289_reset(lcd_generic_ssd1289_t *lcd) {
    EPD_ERROR_CHECK(lcd->cb.reset_cb(lcd->user_data));

    return EPD_OK;
}

static epd_ret_t lcd_generic_ssd1289_window(lcd_generic_ssd1289_t *lcd, epd_coord_t *coord) {
    uint8_t tx_buf[3] = {0x44, 0x00, 0x00}; /* x_end is at higher byte. */
    if (lcd->dir == LCD_GENETIC_SSD1289_DIR_HORIZONTAL) {
        tx_buf[1] = coord->y_end;
        tx_buf[2] = coord->y_start;
    } else {
        tx_buf[1] = coord->x_end;
        tx_buf[2] = coord->x_start;
    }
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, tx_buf, 0x03));

    tx_buf[0] = 0x45;
    if (lcd->dir == LCD_GENETIC_SSD1289_DIR_HORIZONTAL) {
        tx_buf[1] = coord->x_start >> 8U;
        tx_buf[2] = coord->x_start & 0xFFU;
    } else {
        tx_buf[1] = coord->y_start >> 8U;
        tx_buf[2] = coord->y_start & 0xFFU;
    }

    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, tx_buf, 0x03));

    tx_buf[0] = 0x46;
    if (lcd->dir == LCD_GENETIC_SSD1289_DIR_HORIZONTAL) {
        tx_buf[1] = coord->x_end >> 8U;
        tx_buf[2] = coord->x_end & 0xFFU;
    } else {
        tx_buf[1] = coord->y_end >> 8U;
        tx_buf[2] = coord->y_end & 0xFFU;
    }

    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, tx_buf, 0x03));

    return EPD_OK;
}

epd_ret_t lcd_generic_ssd1289_init(lcd_generic_ssd1289_t *lcd) {
    EPD_ERROR_CHECK(lcd_generic_ssd1289_reset(lcd));
    EPD_ERROR_CHECK(epd_common_execute_sequence(&lcd->cb, lcd->user_data, lcd_generic_ssd1289_init_sequence,
                                                sizeof(lcd_generic_ssd1289_init_sequence)));

    uint8_t tx_buf[3] = {0x11, 0x60, 0x30}; /* Entry Mode, DFM[1:0] = 0b10 */
    if (lcd->mode != LCD_GENERIC_SSD1289_MODE_RGB565) {
        tx_buf[1] = 0x40;
        tx_buf[2] |= (lcd->mode & 0x03) << 6U; /* TY[1:0] */
    }

    if (lcd->dir == LCD_GENERIC_SSD1289_DIR_VERTICAL) {
        tx_buf[2] |= 1 << 3U;
    }

    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, tx_buf, 3U));

    return EPD_OK;
}

epd_ret_t lcd_generic_ssd1289_upload(lcd_generic_ssd1289_t *lcd, epd_coord_t *coord, uint8_t *data) {
    EPD_ERROR_CHECK(lcd_generic_ssd1289_window(lcd, coord));

    uint8_t tx_buf[1] = {0x22};
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, tx_buf, 0x01));

    uint32_t pix_size = (coord->x_end - coord->x_start + 1) * (coord->y_end - coord->y_start + 1);

    if (lcd->mode == LCD_GENERIC_SSD1289_MODE_RGB565) {
        pix_size = pix_size * 2;
    } else if (lcd->mode == LCD_GENERIC_SSD1289_MODE_RGB888) {
        pix_size = pix_size * 3;
    } else {
        pix_size = pix_size * 4;
    }

    EPD_ERROR_CHECK(lcd->cb.write_data_cb(lcd->user_data, data, pix_size));

    return EPD_OK;
}