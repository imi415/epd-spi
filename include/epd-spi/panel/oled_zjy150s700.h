#ifndef OLED_PANEL_ZJY150S700_H
#define OLED_PANEL_ZJY150S700_H

#include "epd-spi/epd_common.h"

/**
 * @brief Futaba zjy150s700(R) PMOLED
 * Driver/Controller: Solomon Systech SSD1327
 * Resolution: 128x128@4bpp(16 grayscale)
 * Link: https://www.futaba.com/oled/
 * Datasheet: https://www.futaba.com/wp-content/uploads/2021/08/zjy150s700.pdf
 */

typedef struct {
    epd_cb_t cb;
    void    *user_data;
} oled_zjy150s700_t;

epd_ret_t oled_zjy150s700_init(oled_zjy150s700_t *elw);
epd_ret_t oled_zjy150s700_upload(oled_zjy150s700_t *oled, epd_coord_t *coord, uint8_t *data);
epd_ret_t oled_zjy150s700_power(oled_zjy150s700_t *elw, uint8_t on);

#endif