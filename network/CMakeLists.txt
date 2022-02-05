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
        )

if (APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl@3)
endif ()

find_package(OpenSSL REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${OPENSSL_INCLUDE_DIR}
)

set(MAIN_LIBS logging)

add_library(${PROJECT_NAME} ${SRCS_G})
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS} ${OPENSSL_LIBRARIES})

add_subdirectory(example)
add_subdirectory(testsuite)
