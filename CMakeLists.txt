# SPDX-FileCopyrightText: 2020-2026 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.25)

project(piejam VERSION 0.9.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 26)

option(PIEJAM_BENCHMARKS "Build benchmarks" OFF)
option(PIEJAM_TESTS "Build tests" OFF)

# How to run with coverage:
# cmake -DCMAKE_BUILD_TYPE=Debug -DPIEJAM_TESTS=ON -DPIEJAM_COVERAGE=ON -B build
# cmake --build build
# ctest --output-on-failure
# gcovr -r . build --html --html-details -o coverage.html
option(PIEJAM_COVERAGE "Enable code coverage" OFF)


add_library(piejam_compiler_warnings INTERFACE)

# GNU/Clang warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(piejam_compiler_warnings INTERFACE
        -Wall
        -Wextra
        -Werror
        -pedantic-errors

        -Walloca
        -Wcast-align
        -Wcast-qual
        -Wcomma-subscript
        -Wctor-dtor-privacy
        -Wdeprecated-copy-dtor
        -Wdouble-promotion
        -Wduplicated-branches
        -Wduplicated-cond
        -Wenum-conversion
        -Wformat-overflow=2
        -Wformat-signedness
        -Wformat=2
        -Wlogical-op
        -Wmismatched-tags
        -Wmultichar
        #-Wnoexcept
        -Wnon-virtual-dtor
        -Wpointer-arith
        -Wrange-loop-construct
        -Wrestrict
        -Wstrict-null-sentinel
        -Wsuggest-attribute=format
        -Wsuggest-attribute=malloc
        -Wuninitialized
        -Wno-maybe-uninitialized # should be fixed in gcc-16.2.0
        -Wvla
        -Wvolatile
        -Wwrite-strings

        -Wno-psabi
    )
endif()

if(PIEJAM_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_library(gtest_driver INTERFACE)
    target_link_libraries(gtest_driver INTERFACE gtest gtest_main pthread)

    if(PIEJAM_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-O0 -g --coverage)
        add_link_options(--coverage)
    endif()
endif()

find_package(Boost CONFIG)
find_package(spdlog REQUIRED)

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
