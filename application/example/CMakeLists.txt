cmake_minimum_required(VERSION 3.12)
project(application-example VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G
        src/*.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../src
)

set(APP_EXAMPLE_MAIN_LIBS application)

add_executable(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${APP_EXAMPLE_MAIN_LIBS})

