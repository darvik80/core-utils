cmake_minimum_required(VERSION 3.12)
project(core_utils)

set(CMAKE_CXX_STANDARD 17)

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
        /usr/include
        /usr/local/include
        /usr/local/opt/openssl/include
        logging/src
        network/src
        event-manager/src
        application/src
        iot-platform/src
)

link_directories(
        /usr/local/lib
        /usr/local/Cellar/icu4c/70.1/lib
)

add_subdirectory(logging)
#add_subdirectory(audioplayer)
add_subdirectory(event-manager)
add_subdirectory(serial)
add_subdirectory(network)
add_subdirectory(application)
add_subdirectory(iot-platform)
