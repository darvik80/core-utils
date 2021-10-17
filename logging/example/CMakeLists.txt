cmake_minimum_required(VERSION 3.12)
project(logging-example)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits -Wno-psabi")
endif ()

add_definitions(-DRASPBERRY_ARCH)

file(GLOB SRCS_G
        src/*.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../src
)

add_executable(${PROJECT_NAME} ${SRCS_G})
target_link_libraries(${PROJECT_NAME} logging)
