cmake_minimum_required(VERSION 3.10)
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
