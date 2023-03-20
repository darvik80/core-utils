cmake_minimum_required(VERSION 3.16)
project(core_utils VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# libs {
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
add_definitions(-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DBOOST_STACKTRACE_USE_ADDR2LINE)

find_package(Boost 1.81 REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)
# }

include_directories(
        ${fmt_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${nlohmann_json_INCLUDE_DIRS}
        ${OpenSSL_INCLUDE_DIRS}
        logging/src
        network/src
        event-bus/src
        application/src
        iot-platform/src
)

add_subdirectory(logging)
#add_subdirectory(audioplayer)
add_subdirectory(event-bus)
add_subdirectory(network)
add_subdirectory(application)
add_subdirectory(iot-platform)
add_subdirectory(joystick)
