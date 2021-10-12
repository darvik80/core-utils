cmake_minimum_required(VERSION 3.10)
project(network)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost REQUIRED COMPONENTS system filesystem thread log log_setup)
find_package(Threads)

include_directories(
        /usr/local/include
        ${PROJECT_SOURCE_DIR}/../logging/src
)

link_directories(
        /usr/local/lib
        /usr/local/Cellar/icu4c/69.1/lib
)

file(GLOB SRCS_G src/*.cpp src/network/*.cpp)

set(MAIN_LIBS ${Boost_LIBRARIES} logging)


add_executable(${PROJECT_NAME} ${SRCS_G})
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})
