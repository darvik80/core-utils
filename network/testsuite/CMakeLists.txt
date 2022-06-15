cmake_minimum_required(VERSION 3.0)
project(network_testsuite VERSION 1.0)

file(GLOB TEST_SRCS
        src/*.cpp
        src/zeromq/v30/*.cpp
        src/zeromq/v31/*.cpp
        src/mqtt/v31/*.cpp
        src/mqtt/*.cpp
        src/boost/*.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/../src
)

find_package (Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(${PROJECT_NAME} ${TEST_SRCS})
target_link_libraries(${PROJECT_NAME} PUBLIC logging network ${Boost_LIBRARIES})

enable_testing()
add_test(NAME ${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_NAME})