#include "epd_panel_wfh0420cz35.h"

#include "epd_private.h"

#define DTM1 0x10
#define DTM2 0x13

static uint8_t epd_wfh0420_init_sequence[] = {
    0x03, 0x06, 0x17, 0x17, 0x17,
};

/**
 * @brief This function initialize the driver IC and
 * enable internal power supplies.
 *
 * @param epd pointer to epd_wfh0420_t
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_wfh0420_init(epd_wfh0420_t *epd) {
    EPD_ASSERT(epd);
    EPD_ASSERT(epd->cb.reset_cb);
    EPD_ASSERT(epd->cb.write_command_cb);
    EPD_ASSERT(epd->cb.poll_busy_cb);

    EPD_ERROR_CHECK(epd->cb.reset_cb(epd->user_data));

    EPD_ERROR_CHECK(epd_common_execute_sequence(&epd->cb, epd->user_data, epd_wfh0420_init_sequence,
                                                sizeof(epd_wfh0420_init_sequence)));

    // Issue power on command
    uint8_t tx_buf[2] = {0x04, 0x00};
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x01));

    EPD_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    // Issue panel setting command, LUT from OTP.
    tx_buf[0] = 0x00;
    tx_buf[1] = 0x0F;
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x02));

    return EPD_OK;
}

/**
 * @brief Enable partial mode for UC8176 IC
 * and set partial mode window.
 *
 * @param epd pointer to epd_wfh0420_t
 * @param coord pointer to epd_coord_t
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_wfh0420_partial(epd_wfh0420_t *epd, epd_coord_t *coord) {
    EPD_ASSERT(epd);
    EPD_ASSERT(epd->cb.write_command_cb);

    // Partial In
    uint8_t tx_buf[10] = {0x91, 0x00};
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 1));

    // Partial window
    tx_buf[0] = 0x90;
    tx_buf[1] = (coord->x_start >> 8U) & 0x01U;
    tx_buf[2] = coord->x_start & 0xF8U;  // Bank number, last 3 LSBs must be zero.
    tx_buf[3] = (coord->x_end >> 8U) & 0x01U;
    tx_buf[4] = coord->x_end & 0xF8U;  // Same as above.
    tx_buf[5] = (coord->y_start >> 8U) & 0x01U;
    tx_buf[6] = coord->y_start & 0xFFU;
    tx_buf[7] = (coord->y_end >> 8U) & 0x01U;
    tx_buf[8] = coord->y_end & 0xFFU;
    tx_buf[9] = 0x01;  // Only scan partial area.
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 10));

    return EPD_OK;
}

/**
 * @brief Issue UC8176 DTM1 or DTM2 command, with payload data and stop command.
 *
 * @param epd pointer to epd_wfh0420_t
 * @param dtm_num DTM1 or DTM2
 * @param data payload pixel data
 * @param len payload data length, calculated as bytes
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_wfh0420_dtm(epd_wfh0420_t *epd, uint8_t dtm_num, uint8_t *data, uint32_t len) {
    EPD_ASSERT(epd);
    EPD_ASSERT(epd->cb.write_command_cb);

    uint8_t tx_buf[2] = {dtm_num};

    // Issue DTM1 or DTM2 command
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x01));
    // Send data
    EPD_ERROR_CHECK(epd->cb.write_data_cb(epd->user_data, data, len));
    // Issue DSP command.
    tx_buf[0] = 0x11;
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x01));

    return EPD_OK;
}

/**
 * @brief Issue display update command and enter deep-sleep mode.
 * Deep-sleep mode can only exits with a hardware reset.
 * @param epd pointer to epd_wfh0420_t
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_wfh0420_update_and_sleep(epd_wfh0420_t *epd) {
    EPD_ASSERT(epd);
    EPD_ASSERT(epd->cb.write_command_cb);
    EPD_ASSERT(epd->cb.delay_cb);

    // Issue DRF command
    uint8_t tx_buf[2] = {0x12, 0x00};
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x01));

    // Delay 100ms and wait for busy
    EPD_ERROR_CHECK(epd->cb.delay_cb(epd->user_data, 100));
    EPD_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    // Issue POF command
    tx_buf[0] = 0x02;
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x01));
    EPD_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    // Issue DSLP command
    tx_buf[0] = 0x07;
    tx_buf[1] = 0xA5;
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x02));

    return EPD_OK;
}

/**
 * @brief Initialize display, upload frame buffer and refresh.
 * Then goes to deep-sleep mode to conserve power.
 * This is the only function needs to be called by the user.
 *
 * @param epd pointer to epd_wfh0420_t
 * @param coord pointer to epd_coord_t, x_start or x_end MUST be wrapped to 8 pixel boundaries.
 * @param red_data red pixel data array, bit 0 for red, bit 1 for white
 * @param bw_data black pixel data array, bit 0 for black, bit 1 for white
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
epd_ret_t epd_wfh0420_upload(epd_wfh0420_t *epd, epd_coord_t *coord, uint8_t *red_data, uint8_t *bw_data) {
    EPD_ERROR_CHECK(epd_wfh0420_init(epd));
    EPD_ERROR_CHECK(epd_wfh0420_partial(epd, coord));

    uint8_t  bank_start = coord->x_start / 8;  // Wrap to bank
    uint8_t  bank_end   = coord->x_end / 8;    // Wrap to bank
    uint32_t tx_len     = (bank_end - bank_start + 1) * (coord->y_end - coord->y_start + 1);

    EPD_ERROR_CHECK(epd_wfh0420_dtm(epd, DTM1, bw_data, tx_len));
    EPD_ERROR_CHECK(epd_wfh0420_dtm(epd, DTM2, red_data, tx_len));

    EPD_ERROR_CHECK(epd_wfh0420_update_and_sleep(epd));

    return EPD_OK;
}