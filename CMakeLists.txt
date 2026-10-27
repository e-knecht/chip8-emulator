cmake_minimum_required(VERSION 3.16)
project(chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

add_executable(chip8 main.cpp chip8.cpp)
target_compile_options(chip8 PRIVATE -Wall)
target_link_libraries(chip8 PRIVATE SDL2::SDL2)
