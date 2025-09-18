idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES bt nvs_flash esp_timer driver esp_lcd fatfs sdmmc)