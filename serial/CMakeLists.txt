cmake_minimum_required(VERSION 3.10)
project(serial)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${PROJECT_SOURCE_DIR}/../logging/src
)


file(GLOB SRCS_LIB_G
        src/serial/*.cpp
        )

file(GLOB SRCS_G
        ${SRCS_LIB_G}
        src/*.cpp
        )

# libs {
find_package(Boost REQUIRED COMPONENTS system)

if (APPLE)
    message("Apple build")
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_LIBRARY CoreFoundation)

    set(OS_LIBS ${IOKIT_LIBRARY} ${CORE_LIBRARY})
else ()
    message("Unix build")
    set(OS_LIBS dl)
endif ()


set(MAIN_LIBS ${Boost_LIBRARIES} ${OS_LIBS} logging)
# }


add_library(${PROJECT_NAME} ${SRCS_LIB_G})
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})

add_subdirectory(example)
