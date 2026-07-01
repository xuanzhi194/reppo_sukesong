set(include_dirs
                ./

)
 
set(priv_requires
                    driver
                    esp_timer
                    esp_driver_ledc
                    BSP
                    lvgl
                    lv_port
                    ui_custom 
                    spiffs
        
)
 
idf_component_register(
                        SRCS "main.c"
                               "app_globals.c"
                               "app_init.c"
                               "app_main_loop.c"
                               "app_ui.c"
                               "app_led.c"
                               "app_nvs_worker.c"
                               "app_wifi_worker.c"
                               "wificfg_bridge.c"
                               "wordcard_bridge.c"
                               "volume_bridge.c"
                               "brightness_bridge.c"
                               "focus_bridge.c"
                               "picture_bridge.c"
                        INCLUDE_DIRS ${include_dirs} 
                        PRIV_REQUIRES ${priv_requires}
                        EMBED_FILES  ${embed_files}
)

spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

component_compile_options(
                           -O3
                           -ffast-math
                           -Wno-error=format=-Wno-format
)