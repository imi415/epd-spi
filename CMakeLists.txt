cmake_minimum_required(VERSION 3.10)

project(epd-spi)

set(EPD_SOURCES
    "src/epd_common.c"
    "src/driver/oled_ssd1327.c"
    "src/panel/epd_wfh0420cz35.c"
    "src/panel/epd_gdew042t2.c"
    "src/panel/lcd_gv6416gfsl_011.c"
    "src/panel/lcd_jlx256128g_920.c"
    "src/panel/oled_elw1501aa.c"
    "src/panel/oled_zjy150s700.c"
)

set(EPD_INCLUDES
    "include"
)

set(EPD_PRIVATE_INCLUDES
    "include/epd-spi"
    "include/private"
)

add_library(${PROJECT_NAME} ${EPD_SOURCES})
target_include_directories(${PROJECT_NAME}
    PUBLIC ${EPD_INCLUDES}
    PRIVATE ${EPD_PRIVATE_INCLUDES}
)
