SET(SOURCES screens/ui_Screen2.c
    screens/ui_HOME.c
    screens/ui_Wifi_not_connect.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_power_png.c
    images/ui_img_time_png.c
    images/ui_img_currency_png.c
    images/ui_img_1810231589.c
    fonts/ui_font_Kanit.c)

add_library(ui ${SOURCES})
