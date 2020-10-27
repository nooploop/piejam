cmake_minimum_required(VERSION 3.12)

project(piejam VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

option(PIEJAM_BUILD_AUDIO_TEST "Build pjaudiotest" OFF)
option(PIEJAM_BENCHMARKS "Build benchmarks" OFF)
option(PIEJAM_TESTS "Build tests" OFF)

if(PIEJAM_TESTS)
    enable_testing()
endif()

find_package(Boost)
find_package(spdlog)
find_package(fmt)

add_library(nod INTERFACE)
target_include_directories(nod SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/nod/include)

set(PIEJAM_QML_IMPORT_PATHS "")

add_subdirectory(piejam_libs)
add_subdirectory(piejam_app)

set(QML_IMPORT_PATH ${PIEJAM_QML_IMPORT_PATHS} CACHE STRING "Path used to locate CMake modules by Qt Creator" FORCE)

if(PIEJAM_BUILD_AUDIO_TEST)
    add_subdirectory(pjaudiotest)
endif()
