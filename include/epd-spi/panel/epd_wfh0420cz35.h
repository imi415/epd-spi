#ifndef EPD_WFH0420CZ35_H
#define EPD_WFH0420CZ35_h

#include "epd-spi/epd_common.h"

/**
 * @brief This display module has UC8176 controller IC, with BWR LUT stored in OTP.
 *
 */

typedef struct {
    void    *user_data;
    epd_cb_t cb;
} epd_wfh0420_t;

epd_ret_t epd_wfh0420_upload(epd_wfh0420_t *epd, epd_coord_t *coord, uint8_t *red_data, uint8_t *bw_data);

#endif