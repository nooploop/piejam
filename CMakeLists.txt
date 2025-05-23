# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.25)

project(piejam VERSION 0.9.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

option(PIEJAM_BENCHMARKS "Build benchmarks" OFF)
option(PIEJAM_TESTS "Build tests" OFF)

if(PIEJAM_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_library(gtest_driver INTERFACE)
    target_link_libraries(gtest_driver INTERFACE gtest gtest_main pthread)
endif()

find_package(Boost CONFIG)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_search_module(FFTW3 REQUIRED fftw3f)
add_library(FFTW::Float INTERFACE IMPORTED)
set_target_properties(FFTW::Float
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW3_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${FFTW3_LINK_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${FFTW3_CFLAGS}"
)

pkg_search_module(SNDFILE REQUIRED sndfile)
add_library(SndFile::sndfile INTERFACE IMPORTED)
set_target_properties(SndFile::sndfile
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SNDFILE_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${SNDFILE_LINK_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${SNDFILE_CFLAGS}"
)

add_library(nod INTERFACE)
target_include_directories(nod SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/nod/include)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/MIPP")
set(MIPP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/libs/MIPP/include")
find_package(MIPP REQUIRED)
target_compile_definitions(MIPP INTERFACE MIPP_ALIGNED_LOADS)
set_target_properties(MIPP PROPERTIES SYSTEM ON)

set(PIEJAM_QML_IMPORT_PATHS "")

add_subdirectory(piejam_libs)
add_subdirectory(piejam_app)

set(QML_IMPORT_PATH ${PIEJAM_QML_IMPORT_PATHS} CACHE STRING "Path used to locate CMake modules by Qt Creator" FORCE)
