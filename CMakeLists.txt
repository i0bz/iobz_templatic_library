cmake_minimum_required(VERSION 3.28.0)
project(i0bz_templatic_library VERSION 0.1.0 LANGUAGES CXX)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(i0bz_templatic_library)

target_sources(i0bz_templatic_library
                PUBLIC
                FILE_SET CXX_MODULES
                FILES
                    src/itl-linked_list.cppm)

add_executable(test2 test.cpp)

target_link_libraries(test2 PRIVATE i0bz_templatic_library)


include(CTest)
enable_testing()

