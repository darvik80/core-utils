cmake_minimum_required(VERSION 3.10)
project(audioplayer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/audioplayer/*.cpp)

# libs {
set(MAIN_LIBS logging mpg123 ao)
# }

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})

add_subdirectory(example)
