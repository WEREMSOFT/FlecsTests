cmake_minimum_required(VERSION 3.15)
project(flecsTest C)

file(COPY assets DESTINATION ./)
set(CMAKE_C_STANDARD 11)
FILE(GLOB sources src/*)

link_directories ("${PROJECT_SOURCE_DIR}/libs")
set(CMAKE_C_FLAGS "-lflecs_static ${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS} -framework OpenGL -framework Cocoa")

add_executable(flecsTest ${sources})
target_include_directories(flecsTest PRIVATE include)

