#ifndef EPD_GDEW042T2_H
#define EPD_GDEW042T2_H

#include "epd-spi/epd_common.h"

/**
 * @brief This display module has UC8176 controller IC,
 * with BW partial or 4 grayscale LUT.
 *
 */

typedef enum {
    EPD_GDEW042T2_MODE_GS      = 0U, /* Grayscale mode */
    EPD_GDEW042T2_MODE_BW      = 1U, /* B/W mode */
    EPD_GDEW042T2_MODE_BW_PART = 2U, /* B/W mode with partial flush */
} epd_gdew042t2_mode_t;

typedef struct {
    void                *user_data;
    epd_gdew042t2_mode_t mode;
    epd_cb_t             cb;
} epd_gdew042t2_t;

epd_ret_t epd_gdew042t2_upload(epd_gdew042t2_t *epd, epd_coord_t *coord, uint8_t *data1, uint8_t *data2);

#endif