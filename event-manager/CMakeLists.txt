cmake_minimum_required(VERSION 3.12)
project(event-manager)

set(CMAKE_CXX_STANDARD 17)

# Include Conan info
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SRCS_G src/*.cpp src/event/*.cpp src/scheduler/*.cpp)

add_subdirectory(example)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} logging)
