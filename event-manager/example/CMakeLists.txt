cmake_minimum_required(VERSION 3.12)
project(event-manager-example VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G
        src/*.cpp
        ../src/resource/*.cpp
        ../src/scheduler/*.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../src
)

set(EM_EXAMPLE_MAIN_LIBS logging event-manager)

add_executable(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${EM_EXAMPLE_MAIN_LIBS})

