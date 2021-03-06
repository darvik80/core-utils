cmake_minimum_required(VERSION 3.12)
project(core_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits -Wno-psabi")
endif ()

set(Boost_USE_STATIC_LIBS OFF)  # only find static libs
set(Boost_USE_SHARED_LIBS ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON)  # only find release libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

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
