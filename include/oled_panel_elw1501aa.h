#ifndef OLED_PANEL_EWL1501AA_H
#define OLED_PANEL_EWL1501AA_H

#include "epd_common.h"

typedef struct {
    epd_cb_t cb;
    void *user_data;
} oled_ewl1501aa_t;

epd_ret_t oled_ewl1501aa_init(oled_ewl1501aa_t *ewl);
epd_ret_t oled_ewl1501aa_upload(oled_ewl1501aa_t *oled, epd_coord_t *coord, uint8_t *data);
epd_ret_t oled_ewl1501aa_power(oled_ewl1501aa_t *ewl, uint8_t on);

#endif