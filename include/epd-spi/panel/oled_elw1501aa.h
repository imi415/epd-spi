#ifndef OLED_PANEL_ELW1501AA_H
#define OLED_PANEL_ELW1501AA_H

#include "epd-spi/epd_common.h"

/**
 * @brief Futaba ELW1501AA(R) PMOLED
 * Driver/Controller: Solomon Systech SSD1327
 * Resolution: 128x128@4bpp(16 grayscale)
 * Link: https://www.futaba.com/oled/
 * Datasheet: https://www.futaba.com/wp-content/uploads/2021/08/ELW1501AA.pdf
 */

typedef struct {
    epd_cb_t cb;
    void    *user_data;
} oled_elw1501aa_t;

epd_ret_t oled_elw1501aa_init(oled_elw1501aa_t *elw);
epd_ret_t oled_elw1501aa_upload(oled_elw1501aa_t *oled, epd_coord_t *coord, uint8_t *data);
epd_ret_t oled_elw1501aa_power(oled_elw1501aa_t *elw, uint8_t on);

#endif