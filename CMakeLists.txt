# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.12)

project(piejam VERSION 0.7.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

option(PIEJAM_BENCHMARKS "Build benchmarks" OFF)
option(PIEJAM_COVERAGE "Enable test coverage" OFF)
option(PIEJAM_TESTS "Build tests" OFF)
option(PIEJAM_TOOLS "Build tools" OFF)

if(PIEJAM_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_library(gtest_driver INTERFACE)
    target_link_libraries(gtest_driver INTERFACE gtest gtest_main pthread)
endif()

find_program(PIEJAM_LCOV_PATH lcov)
find_program(PIEJAM_GENHTML_PATH genhtml)
if(PIEJAM_COVERAGE AND PIEJAM_LCOV_PATH AND PIEJAM_GENHTML_PATH)
    set(PIEJAM_COVERAGE_ENABLED ON)
    add_compile_options(--coverage)
    add_link_options(--coverage)

    set(PIEJAM_COVERAGE_IGNORE_LIST
      "*/tests/*"
      "*/benchmarks/*"
      "*moc_*.cpp"
      "*qrc_*.cpp"
      "*/export_graph_as_dot.cpp"
      "*/mocs_compilation.cpp"
      )
    list(TRANSFORM PIEJAM_COVERAGE_IGNORE_LIST REPLACE "(.+)" "--exclude '\\1'" OUTPUT_VARIABLE PIEJAM_COVERAGE_IGNORE_LIST_EX)
    list(JOIN PIEJAM_COVERAGE_IGNORE_LIST_EX " " PIEJAM_COVERAGE_IGNORE)
else()
    set(PIEJAM_COVERAGE_ENABLED OFF)
endif()
configure_file("CTestCustom.cmake.in" "CTestCustom.cmake")

find_package(Boost COMPONENTS program_options)
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

add_library(mipp INTERFACE)
target_include_directories(mipp SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/MIPP/src)
target_compile_definitions(mipp INTERFACE MIPP_ALIGNED_LOADS)

set(PIEJAM_QML_IMPORT_PATHS "")

add_subdirectory(piejam_libs)
add_subdirectory(piejam_app)

set(QML_IMPORT_PATH ${PIEJAM_QML_IMPORT_PATHS} CACHE STRING "Path used to locate CMake modules by Qt Creator" FORCE)

if(PIEJAM_TOOLS)
    add_subdirectory(piejam_tools)
endif()
