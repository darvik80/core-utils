cmake_minimum_required(VERSION 3.12)
project(event-manager)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/event/*.cpp src/scheduler/*.cpp)

include_directories(
        ../logging/src
)

set(EVENT_MAIN_LIBS logging)

add_subdirectory(example)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${EVENT_MAIN_LIBS})
