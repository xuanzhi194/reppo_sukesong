set(src_dirs         
            LCD
            keypad
            wifi
            todo_storage
            buzzer
            bat_percent
            RX8130
            RGB
            encoder
            bsp_ble
            badge_storage
 
)
 
set(include_dirs            
            LCD
            keypad
            wifi
            todo_storage
            buzzer
            bat_percent
            RX8130
            RGB
            encoder
            bsp_ble
            badge_storage
)
 
set(requires
            driver
            esp_timer
            esp_driver_ledc
            esp_lcd
            esp_wifi 
            esp_event 
            esp_netif
            nvs_flash
            esp_system
            spiffs
            fatfs
            wear_levelling
            esp_adc
            led_strip
            bt
            json
            mbedtls

)
 
idf_component_register(
                        SRC_DIRS ${src_dirs}
                        INCLUDE_DIRS ${include_dirs} 
                        REQUIRES ${requires}
)
 
component_compile_options(
                            -ffast-math 
                            -O3
                            -Wno-error=format=-Wno-format
                            -Werror=implicit-function-declaration
)
