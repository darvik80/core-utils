cmake_minimum_required(VERSION 3.16)
project(event-bus)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/*.cpp src/event/*.cpp)

add_subdirectory(example)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} logging ${fmt_LIBRARIES} ${Boost_LIBRARIES})
