cmake_minimum_required(VERSION 3.12)
project(iot-platform)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/iot/*.cpp)

set(IOT_MAIN_LIBS application network)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${IOT_MAIN_LIBS})
