cmake_minimum_required(VERSION 3.12)
project(logging)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/*.cpp src/logging/*.cpp)

# Include Conan info
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# libs {
add_definitions(-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DBOOST_STACKTRACE_USE_ADDR2LINE)
find_package(Boost REQUIRED COMPONENTS system filesystem thread log log_setup REQUIRED)
# }

add_subdirectory(example)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_LIBS} ${Boost_LIBRARIES})
