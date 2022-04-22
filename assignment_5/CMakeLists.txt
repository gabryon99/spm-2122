cmake_minimum_required(VERSION 3.20)    # CMake version check
project(assignment
    VERSION 0.1
    DESCRIPTION "Template assignment of SPM course (2021/2022)"
)                                        # Create project "assignment"

option(DEBUG "Enable/disable debug" ON)

set(SPM_ASSIGNMENT 5)       # Number of the current assignment
set(CMAKE_CXX_STANDARD 20)  # Enable C++20 standard

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*;") # Enable clang-tidy

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/main.cpp)

configure_file(config/assignmentconfig.h.in assignmentconfig.h @ONLY)

if (DEBUG)
    add_compile_options(-g -O2) 
else()
    add_compile_options(-O3 -Wall -pedantic) 
endif()

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(assignment ${SOURCE_FILES})

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(assignment PUBLIC OpenMP::OpenMP_CXX)
endif()

target_include_directories(assignment PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}
    ./include/
    ../common/include/
    ../common/library/argparse/include)