#include "panel/lcd_gv6416gfsl_011.h"

#include "epd_private.h"

/* *16 */

static uint8_t lcd_gv6416g_init_sequence[] = {
    0x00, 0x01,                                      // Software reset (0x01)
    0x00, 0x11,                                      // Sleep OUT (0x11)
    0x00, 0x20,                                      // Inverse display OFF (0x20)
    0x00, 0x22,                                      // Exit all point on (0x22)
    0x01, 0xB0, 0x03,                                // Duty set (0xB0)
    0x01, 0xB1, 0x02,                                // LED mode (0xB1)
    0x01, 0xB2, 0x14,                                // Frame frequency (0xB2)
    0x04, 0xB5, 0x05, 0x00, 0x00, 0x00,              // Driver mode (0xB5)
    0x06, 0xB6, 0x02, 0x02, 0x02, 0x60, 0x60, 0x60,  // LED waveform set (0xB6)
    0x01, 0xB7, 0x00,                                // LCD scan set (0xB7)
    0x02, 0xC0, 0x70, 0x00,                          // Vop set (0xC0)
    0x01, 0xC3, 0x00,                                // Bias selection (0xC3)
    0x01, 0xD2, 0x00,                                // Power control (0xD2)
    0x01, 0xD4, 0x00,                                // RGB LED control (0xD4)
    0x00, 0x29,                                      // Display ON (0x29)
};

epd_ret_t lcd_gv6416g_init(lcd_gv6416g_t *lcd) {
    EPD_ERROR_CHECK(epd_common_execute_sequence(&lcd->cb, lcd->user_data, lcd_gv6416g_init_sequence,
                                                sizeof(lcd_gv6416g_init_sequence)));

    return EPD_OK;
}

epd_ret_t lcd_gv6416g_upload(lcd_gv6416g_t *lcd, epd_coord_t *coord, uint8_t *data) {
    uint8_t cmd[3] = { 0x2A, 0x10, 0x8F };
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, cmd, 0x03));

    cmd[0] = 0x2C;
    EPD_ERROR_CHECK(lcd->cb.write_command_cb(lcd->user_data, cmd, 0x01));

    EPD_ERROR_CHECK(lcd->cb.write_data_cb(lcd->user_data, data, 512));

    return EPD_OK;
}