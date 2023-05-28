cmake_minimum_required(VERSION 3.10)
project(core)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB SRCS_G
        src/*.cpp
        )

add_library(${PROJECT_NAME} ${SRCS_G})
target_link_libraries(${PROJECT_NAME} logging)
