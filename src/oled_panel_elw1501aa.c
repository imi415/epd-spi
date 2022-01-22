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

static epd_ret_t oled_ewl1501aa_window(oled_ewl1501aa_t *oled, epd_coord_t *coord) {
    if (coord->x_start % 2 != 0 || coord->x_end % 2 == 0) {
        return EPD_FAIL;
    }

    if (coord->x_end > 127 || coord->x_start > coord->x_end) {
        return EPD_FAIL;
    }

    if (coord->y_end > 127 || coord->y_start > coord->y_end) {
        return EPD_FAIL;
    }

    uint8_t col_start      = coord->x_start / 2;
    uint8_t col_end        = coord->x_end / 2;

    uint8_t col_cmd_buf[3] = {0x15, col_start, col_end};
    uint8_t row_cmd_buf[3] = {0x75, coord->y_start, coord->y_end};
    EPD_ERROR_CHECK(oled->cb.write_command_cb(oled->user_data, col_cmd_buf, 3));
    EPD_ERROR_CHECK(oled->cb.write_command_cb(oled->user_data, row_cmd_buf, 3));

    return EPD_OK;
}

static inline uint32_t oled_ewl1501aa_data_size(epd_coord_t *coord) {
    return (coord->x_end - coord->x_start + 1) * (coord->y_end - coord->y_start + 1) / 2;
}

/**
 * @brief Initialize OLED panel
 * As OLEDs and LCDs need to be driven constantly, also with
 * less overhead and higher data rate, initialization sequence
 * are moved to this seperate function. Call this function before
 * sending framebuffer data.
 * @param oled pointer to oled_ewl1501aa_t
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
epd_ret_t oled_ewl1501aa_init(oled_ewl1501aa_t *oled) {
    EPD_ERROR_CHECK(oled_ewl1501aa_reset(oled));
    EPD_ERROR_CHECK(epd_common_execute_sequence(&oled->cb, oled->user_data, ewl1501aa_init_sequence,
                                                sizeof(ewl1501aa_init_sequence)));

    return EPD_OK;
}

/**
 * @brief Upload frame buffer to screen.
 *
 * @param oled pointer to oled_ewl1501aa_t
 * @param coord pointer to epd_coord_t, can be NULL to upload full frame.
 * @param data array pointer to new pixel data.
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
epd_ret_t oled_ewl1501aa_upload(oled_ewl1501aa_t *oled, epd_coord_t *coord, uint8_t *data) {
    uint32_t data_size = 8192;
    if (coord != NULL) {
        EPD_ERROR_CHECK(oled_ewl1501aa_window(oled, coord));
        data_size = oled_ewl1501aa_data_size(coord);
    }
    EPD_ERROR_CHECK(oled->cb.write_data_cb(oled->user_data, data, data_size));

    if (coord != NULL) {
        epd_coord_t new_coord = {
            .x_start = 0,
            .x_end   = 127,
            .y_start = 0,
            .y_end   = 127,
        };
        EPD_ERROR_CHECK(oled_ewl1501aa_window(oled, &new_coord));
    }
    return EPD_OK;
}

/**
 * @brief Control the OLED drivers.
 * LCDs and OLEDs needs to be constant driven in order to
 * display a stable image, turn the drivers and source/gate scan on or off.
 *
 * @param oled pointer to oled_ewl1501aa_t
 * @param on 0:off, anything else: on
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
epd_ret_t oled_ewl1501aa_power(oled_ewl1501aa_t *oled, uint8_t on) {
    uint8_t cmd[2] = {0xAE, 0x00};
    if (on) cmd[0] = 0xAF;
    EPD_ERROR_CHECK(oled->cb.write_command_cb(oled->user_data, cmd, 1));

    return EPD_OK;
}