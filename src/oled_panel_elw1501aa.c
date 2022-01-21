#include "oled_panel_elw1501aa.h"

#include "epd_private.h"

static uint8_t ewl1501aa_init_sequence[] = {
    0x01, 0x81, 0x9C,        // Set contrast control
    0x01, 0xA0, 0x53,        // Set segment re-map
    0x01, 0xA1, 0x00,        // Set display start line
    0x01, 0xA2, 0x00,        // Set display offset
    0x00, 0xA4,              // Set normal display mode
    0x01, 0xA8, 0x7F,        // Set MUX ratio
    0x01, 0xAB, 0x01,        // Set enable internal VDD regulator mode
    0x01, 0xB1, 0x31,        // Set phase length of phase 1, phase 2
    0x01, 0xB3, 0xF1,        // Set ratio of dividing frequency & oscillation frequency
    0x00, 0xB9,              // Set gray scale table
    0x01, 0xBC, 0x07,        // Set pre-charge voltage
    0x01, 0xBE, 0x07,        // Set voltage VCOMH
    0x02, 0x15, 0x00, 0x3F,  // Set column address
    0x02, 0x75, 0x00, 0x7F,  // Set row address
};

static epd_ret_t oled_ewl1501aa_reset(oled_ewl1501aa_t *oled) {
    if (oled->cb.reset_cb != NULL) {
        return oled->cb.reset_cb(oled->user_data);
    }

    return EPD_OK;
}

epd_ret_t oled_ewl1501aa_init(oled_ewl1501aa_t *oled) {
    EPD_ERROR_CHECK(oled_ewl1501aa_reset(oled));
    EPD_ERROR_CHECK(epd_common_execute_sequence(&oled->cb, oled->user_data, ewl1501aa_init_sequence,
                                                sizeof(ewl1501aa_init_sequence)));

    return EPD_OK;
}

static epd_ret_t oled_ewl1501aa_window(oled_ewl1501aa_t *oled, epd_coord_t *coord) {
    if (coord->x_start % 2 != 0 || coord->x_end % 2 == 0) {
        return EPD_FAIL;
    }

    return EPD_OK;
}

epd_ret_t oled_ewl1501aa_upload(oled_ewl1501aa_t *ewl, uint8_t *data) {
    EPD_ERROR_CHECK(ewl->cb.write_data_cb(ewl->user_data, data, 8192));
    return EPD_OK;
}

epd_ret_t oled_ewl1501aa_power(oled_ewl1501aa_t *oled, uint8_t on) {
    uint8_t cmd[2] = {0xAE, 0x00};
    if (on) cmd[0] = 0xAF;
    EPD_ERROR_CHECK(oled->cb.write_command_cb(oled->user_data, cmd, 1));

    return EPD_OK;
}