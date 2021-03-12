cmake_minimum_required(VERSION 3.5)

include_directories(components/)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if(ADV_INTERVAL)
add_compile_definitions(ADV_INTERVAL=${ADV_INTERVAL})
endif()

if(PROV_BLE)
add_compile_definitions(PROV_BLE)
endif()

if(SENSOR_PIN)
add_compile_definitions(SENSOR_PIN=${SENSOR_PIN})
endif()

project(apCare)
