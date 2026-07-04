file(GLOB_RECURSE SRCS
     "*.c"
)
idf_component_register(
     SRCS ${SRCS}
     INCLUDE_DIRS . custom generated
                  generated/guider_fonts
                  generated/guider_customer_fonts
                  generated/images
     REQUIRES lvgl
              lv_port
              BSP
)