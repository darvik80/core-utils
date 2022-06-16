cmake_minimum_required(VERSION 3.12)
project(core_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-type-limits -Wno-psabi")
endif ()

include_directories(
        /usr/include
        /usr/local/include
       logging/src
       event-manager/src
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
