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

# libs {
add_definitions(-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DBOOST_STACKTRACE_USE_ADDR2LINE)
if (UNIX)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
else ()
    link_directories(${CONAN_BOOST_ROOT}/lib)
    set(Boost_LIBRARIES libboost_unit_test_framework)
endif ()
# }


add_executable(${PROJECT_NAME} ${TEST_SRCS})
target_link_libraries(${PROJECT_NAME} PUBLIC logging network ${Boost_LIBRARIES})

enable_testing()
add_test(NAME ${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PROJECT_NAME})