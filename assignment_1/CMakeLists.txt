cmake_minimum_required(VERSION 3.13)    # CMake version check
project(assignment_1)                   # Create project "assignment_1"
set(CMAKE_CXX_STANDARD 17)              # Enable C++17 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.cpp)

add_compile_options(-O3 -Wall -pedantic) 

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(assignment_1 ${SOURCE_FILES})