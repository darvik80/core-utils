cmake_minimum_required(VERSION 3.10)
project(audioplayer)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${PROJECT_SOURCE_DIR}/../logging/src
)

file(GLOB SRCS_G
        src/*.cpp
        src/audioplayer/*.cpp
        )

# libs {
set(MAIN_LIBS logging ao mpg123)
# }

add_executable(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})
