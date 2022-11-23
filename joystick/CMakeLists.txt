cmake_minimum_required(VERSION 3.12)
project(joystick)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/joystick/*.cpp)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} application)
