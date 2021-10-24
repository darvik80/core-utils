cmake_minimum_required(VERSION 3.10)
project(network)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DRASPBERRY_ARCH)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB SRCS_G
        src/*.cpp
        src/network/*.cpp
        src/network/handler/*.cpp
        src/network/boost/*.cpp
        src/network/zeromq/*.cpp
        src/network/zeromq/v3.0/*.cpp
        )

set(MAIN_LIBS logging)

add_library(${PROJECT_NAME} ${SRCS_G})
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})

add_subdirectory(example)
