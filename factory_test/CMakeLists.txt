cmake_minimum_required(VERSION 3.5)

# 复用上级项目的 BSP 等组件，以及 IDF 托管组件（led_strip 等）
set(EXTRA_COMPONENT_DIRS "../components" "../managed_components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(factory-test)
