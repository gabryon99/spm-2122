cmake_minimum_required(VERSION 3.20)    # CMake version check
project(assignment_3
    VERSION 0.1
    DESCRIPTION "Third assignment of SPM course (2021/2022)"
)                                        # Create project "assignment_3"

option(DEBUG "Enable/disable debug" ON)

set(SPM_ASSIGNMENT 3) # Number of the current assignment
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")

set(CMAKE_CXX_STANDARD 20)              # Enable C++20 standard

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*;") # Enable clang-tidy

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/main.cpp)

configure_file(config/assignmentconfig.h.in assignmentconfig.h @ONLY)

if (DEBUG)
    add_compile_options(-g) 
else()
    add_compile_options(-O3 -Wall -pedantic) 
endif()

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(assignment_3 ${SOURCE_FILES})

target_include_directories(assignment_3 PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    include/
    library/argparse/include)