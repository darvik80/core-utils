cmake_minimum_required(VERSION 3.12)
project(serial-example VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits -Wno-psabi")
endif ()

file(GLOB SRCS_G
        src/*.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../src
)

set(SERIAL_EXAMPLE_MAIN_LIBS logging serial)

add_executable(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${SERIAL_EXAMPLE_MAIN_LIBS})

