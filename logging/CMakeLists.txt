cmake_minimum_required(VERSION 3.12)
project(logging)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G src/*.cpp src/logging/*.cpp)

# libs {
add_definitions(-DBOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED -DBOOST_STACKTRACE_USE_ADDR2LINE)
if (UNIX)
    find_package(Boost REQUIRED COMPONENTS system filesystem thread log log_setup REQUIRED)
    set(REQ_LIBS fmt ${CMAKE_DL_LIBS})
else()
    link_directories(${CONAN_BOOST_ROOT}/lib)
    set(Boost_LIBRARIES libboost_system libboost_filesystem libboost_thread libboost_log libboost_log_setup)

    set(REQ_LIBS ${CONAN_LIBS})
endif()
# }

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

add_subdirectory(example)

add_library(${PROJECT_NAME} ${SRCS_G})

target_link_libraries(${PROJECT_NAME} ${REQ_LIBS} ${Boost_LIBRARIES})
