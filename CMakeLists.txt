cmake_minimum_required(VERSION 3.12)
project(core_utils)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        /usr/local/include
)

link_directories(
        /usr/local/lib
)

add_subdirectory(logging)
add_subdirectory(audioplayer)
add_subdirectory(event-manager)
add_subdirectory(serial)
