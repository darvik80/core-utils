cmake_minimum_required(VERSION 3.12)
project(logging)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/*.cpp src/logging/*.cpp)

# libs {

set(Boost_USE_STATIC_LIBS       ON)  # only find static libs
set(Boost_USE_SHARED_LIBS       OFF)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

add_definitions(-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DBOOST_STACKTRACE_USE_ADDR2LINE)
find_package(Boost REQUIRED COMPONENTS system filesystem thread log log_setup REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
# }

add_subdirectory(example)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} fmt ${Boost_LIBRARIES} ${CMAKE_DL_LIBS})
