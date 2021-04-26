cmake_minimum_required(VERSION 3.0.0)
project(Chip8 VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE src_files 
    "src/*.h" 
    "src/*.hpp"
    "src/*.cpp")

find_package(SFML 2.5 COMPONENTS graphics window audio system REQUIRED)
add_executable(Chip8 ${src_files})
target_link_libraries(Chip8 sfml-graphics sfml-window sfml-audio sfml-system)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
