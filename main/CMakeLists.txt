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
                    esp_http_client
                    esp-tls
                    lwip
                    json
        
)
 
idf_component_register(
                        SRCS "inspiration_ui.c" "inspiration_storage.c" "inspiration_service.c" "app_time_sync.c" "keep_machine.c" "main.c"
                               "demo_boot_gate.c"
                               "app_globals.c"
                               "app_init.c"
                               "app_main_loop.c"
                               "app_ui.c"
                               "app_led.c"
                               "imu_motion.c"
                               "voice_todo.c"
                               "inspiration_storage.c"
                               "inspiration_service.c"
                               "inspiration_ui.c"
                               "voice_todo_font.c" "voice_todo_title_font.c"
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
                               "tilt_game.c"
                               "tilt_render.c"
                        INCLUDE_DIRS ${include_dirs} 
                        PRIV_REQUIRES ${priv_requires}
                        EMBED_FILES  ${embed_files}
)

# Ordinary firmware updates must preserve user notes, todos and imported books.
# Explicitly opt in only for initial factory provisioning.
option(REPPO_FLASH_FACTORY_STORAGE "Include initial storage image when flashing" OFF)
if(REPPO_FLASH_FACTORY_STORAGE)
    spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
else()
    spiffs_create_partition_image(storage ../data)
endif()
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

component_compile_options(
                           -O3
                           -ffast-math
                           -Wno-error=format=-Wno-format
)
