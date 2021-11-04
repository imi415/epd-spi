#ifndef EPD_PRIVATE_H
#define EPD_PRIVATE_H

#include "epd_common.h"

#define EPD_ASSERT(x) if(!(x)) for(;;) { /* ABORT. */}
#define EPD_ERROR_CHECK(x) if(x != EPD_OK) return EPD_FAIL

epd_ret_t epd_common_execute_sequence(epd_cb_t *cb, void *user_data, uint8_t *seq, uint32_t seq_len);

#endif