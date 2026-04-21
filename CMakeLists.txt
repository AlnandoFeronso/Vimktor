cmake_minimum_required(VERSION 3.10)

project(Vimktor VERSION 0.5)

set(CMAKE_CXX_STANDARD 23)
file(GLOB SOURCES "*.cpp")

add_executable(Vimktor ${SOURCES})

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(Vimktor ${CURSES_LIBRARIES})
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -g")


target_include_directories(Vimktor PUBLIC
                          "${PROJECT_BINARY_DIR}"
			  "${PROJECT_SOURCE_DIR}/include"
                          )
