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
                               "wificfg_bridge.c"
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