#include "panel/epd_gdew042t2.h"

#include "panel/epd_gdew042t2_lut.h"
#include "epd_private.h"

#define DTM1 0x10
#define DTM2 0x13

static uint8_t epd_gdew042t2_init_sequence_bw[] = {
    0x04, 0x01, 0x03, 0x00, 0x2B, 0x2B,  // PWR setting
    0x03, 0x06, 0x17, 0x17, 0x17,        // Boost
};

static uint8_t epd_gdew042t2_init_sequence_gs[] = {
    0x05, 0x01, 0x03, 0x00, 0x2B, 0x2B, 0x13,  // PWR setting
    0x03, 0x06, 0x17, 0x17, 0x17,              // Boost
};

static uint8_t epd_gdew042t2_init_sequence_bw_2[] = {
    0x02, 0x00, 0xBF, 0x0D,              // Panel settings
    0x01, 0x30, 0x3C,                    // PLL
    0x04, 0x61, 0x01, 0x90, 0x01, 0x2C,  // Resolution
    0x01, 0x82, 0x28,                    // VCOM DC
    0x01, 0x50, 0x97,                    // VCOM/Data Interval
};

static uint8_t epd_gdew042t2_init_sequence_gs_2[] = {
    0x01, 0x00, 0x3F,                    // Panel settings
    0x01, 0x30, 0x3C,                    // PLL
    0x04, 0x61, 0x01, 0x90, 0x01, 0x2C,  // Resolution
    0x01, 0x82, 0x12,                    // VCOM DC
    0x01, 0x50, 0x97,                    // VCOM/Data Interval
};

static epd_ret_t epd_gdew042t2_write_lut(epd_gdew042t2_t *epd) {
    uint8_t tx_buf[45];

    tx_buf[0] = 0x20;  // Write VCOM LUT
    memcpy(&tx_buf[1], gdew042t2_lut[epd->mode].VCOM, 44);
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 45));

    tx_buf[0] = 0x21;  // Write WW LUT
    memcpy(&tx_buf[1], gdew042t2_lut[epd->mode].WW, 42);
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 42));

    tx_buf[0] = 0x22;  // Write BW LUT
    memcpy(&tx_buf[1], gdew042t2_lut[epd->mode].BW, 42);
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 42));

    tx_buf[0] = 0x23;  // Write WB LUT
    memcpy(&tx_buf[1], gdew042t2_lut[epd->mode].WB, 42);
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 42));

    tx_buf[0] = 0x24;  // Write BB LUT
    memcpy(&tx_buf[1], gdew042t2_lut[epd->mode].BB, 42);
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 42));

    if (epd->mode == EPD_GDEW042T2_MODE_GS) {
        tx_buf[0] = 0x25;  // Write mysterious WW2 LUT
        memcpy(&tx_buf[1], gdew042t2_lut[epd->mode].WW, 42);
        EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 42));
    }

    return EPD_OK;
}

static epd_ret_t epd_gdew042t2_init(epd_gdew042t2_t *epd) {
    EPD_ASSERT(epd);
    EPD_ASSERT(epd->cb.reset_cb);
    EPD_ASSERT(epd->cb.write_command_cb);
    EPD_ASSERT(epd->cb.poll_busy_cb);

    EPD_ERROR_CHECK(epd->cb.reset_cb(epd->user_data));

    // Pre power-on settings
    if (epd->mode != EPD_GDEW042T2_MODE_GS) {
        EPD_ERROR_CHECK(epd_common_execute_sequence(&epd->cb, epd->user_data, epd_gdew042t2_init_sequence_bw,
                                                    sizeof(epd_gdew042t2_init_sequence_bw)));
    } else {
        EPD_ERROR_CHECK(epd_common_execute_sequence(&epd->cb, epd->user_data, epd_gdew042t2_init_sequence_gs,
                                                    sizeof(epd_gdew042t2_init_sequence_gs)));
    }

    // Issue power on command
    uint8_t tx_buf[2] = {0x04, 0x00};
    EPD_ERROR_CHECK(epd->cb.write_command_cb(epd->user_data, tx_buf, 0x01));

    EPD_ERROR_CHECK(epd->cb.poll_busy_cb(epd->user_data));

    // Post power-on settings
    if (epd->mode != EPD_GDEW042T2_MODE_GS) {
        EPD_ERROR_CHECK(epd_common_execute_sequence(&epd->cb, epd->user_data, epd_gdew042t2_init_sequence_bw_2,
                                                    sizeof(epd_gdew042t2_init_sequence_bw_2)));
    } else {
        EPD_ERROR_CHECK(epd_common_execute_sequence(&epd->cb, epd->user_data, epd_gdew042t2_init_sequence_gs_2,
                                                    sizeof(epd_gdew042t2_init_sequence_gs_2)));
    }

    return EPD_OK;
}

/**
 * @brief Enable partial mode for UC8176 IC
 * and set partial mode window.
 *
 * @param epd pointer to epd_gdew042t2_t
 * @param coord pointer to epd_coord_t
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_gdew042t2_partial(epd_gdew042t2_t *epd, epd_coord_t *coord) {
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
 * @param epd pointer to epd_gdew042t2_t
 * @param dtm_num DTM1 or DTM2
 * @param data payload pixel data
 * @param len payload data length, calculated as bytes
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_gdew042t2_dtm(epd_gdew042t2_t *epd, uint8_t dtm_num, uint8_t *data, uint32_t len) {
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
 * @param epd pointer to epd_gdew042t2_t
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
static epd_ret_t epd_gdew042t2_update_and_sleep(epd_gdew042t2_t *epd) {
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
 * @param epd pointer to epd_gdew042t2_t
 * @param coord pointer to epd_coord_t, x_start or x_end MUST be wrapped to 8 pixel boundaries.
 * @param data1 bw data or old data array
 * @param data2 bw data or new data array
 * @return epd_ret_t EPD_OK for success, EPD_FAIL for error.
 */
epd_ret_t epd_gdew042t2_upload(epd_gdew042t2_t *epd, epd_coord_t *coord, uint8_t *data1, uint8_t *data2) {
    EPD_ERROR_CHECK(epd_gdew042t2_init(epd));
    EPD_ERROR_CHECK(epd_gdew042t2_write_lut(epd));
    EPD_ERROR_CHECK(epd_gdew042t2_partial(epd, coord));

    uint8_t  bank_start = coord->x_start / 8;  // Wrap to bank
    uint8_t  bank_end   = coord->x_end / 8;    // Wrap to bank
    uint32_t tx_len     = (bank_end - bank_start + 1) * (coord->y_end - coord->y_start + 1);

    if (epd->mode != EPD_GDEW042T2_MODE_BW) {
        EPD_ERROR_CHECK(epd_gdew042t2_dtm(epd, DTM1, data1, tx_len));
    }
    EPD_ERROR_CHECK(epd_gdew042t2_dtm(epd, DTM2, data2, tx_len));

    EPD_ERROR_CHECK(epd_gdew042t2_update_and_sleep(epd));

    return EPD_OK;
}
