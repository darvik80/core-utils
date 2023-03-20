cmake_minimum_required(VERSION 3.10)
project(network)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DRASPBERRY_ARCH)

file(GLOB SRCS_G
        src/*.cpp
        src/network/*.cpp
        src/network/handler/*.cpp
        src/network/boost/*.cpp
        src/network/zeromq/*.cpp
        src/network/zeromq/v30/*.cpp
        src/network/zeromq/v31/*.cpp
        src/network/mqtt/*.cpp
        src/network/mqtt/v31/*.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME} ${SRCS_G})
target_link_libraries(${PROJECT_NAME} logging OpenSSL::SSL OpenSSL::Crypto)

add_subdirectory(example)
add_subdirectory(testsuite)
