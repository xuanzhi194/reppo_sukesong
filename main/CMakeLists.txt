set(include_dirs
                ./

)

set(embed_files
                "${CMAKE_CURRENT_LIST_DIR}/../data/book.txt"
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
                    nvs_flash
        
)
 
idf_component_register(
                        SRCS "keep_machine.c" "main.c"
                               "demo_boot_gate.c"
                               "app_globals.c"
                               "app_init.c"
                               "app_main_loop.c"
                               "app_ui.c"
                               "app_led.c"
                               "app_nvs_worker.c"
                               "app_wifi_worker.c"
                               "wificfg_bridge.c"
                               "volume_bridge.c"
                               "brightness_bridge.c"
                               "sleep_bridge.c"
                               "focus_bridge.c"
                               "picture_bridge.c"
                               "gash_machine.c"
                               "book_machine.c"
                               "game_machine.c"
                               "game_score_storage.c"
                               "dino_game.c"
                               "dino_render.c"
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
