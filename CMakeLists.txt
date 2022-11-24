cmake_minimum_required(VERSION 3.12)
project(core_utils)

set(CMAKE_CXX_STANDARD 17)

set(Boost_NO_WARN_NEW_VERSIONS 1)
set(CONAN_DISABLE_CHECK_COMPILER ON)
if (WIN32)
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=2 -DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -fPIC")
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits -Wno-psabi")
    endif()
endif()

include_directories(
        logging/src
        network/src
        event-bus/src
        application/src
        iot-platform/src
)

add_subdirectory(logging)
#add_subdirectory(audioplayer)
add_subdirectory(event-bus)
add_subdirectory(serial)
add_subdirectory(network)
add_subdirectory(application)
add_subdirectory(iot-platform)
add_subdirectory(joystick)
