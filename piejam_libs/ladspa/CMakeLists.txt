# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

include(CheckIncludeFile)
check_include_file("ladspa.h" LADSPA_HEADER)
if(NOT LADSPA_HEADER)
    message(FATAL_ERROR "LADSPA header not found")
endif()

add_library(piejam_ladspa STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/instance_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/instance_manager_processor_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/plugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/plugin_descriptor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/port_descriptor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/processor_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/ladspa/scan.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/ladspa/instance_manager_processor_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/ladspa/plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/ladspa/scan.cpp
)

target_compile_options(piejam_ladspa PRIVATE -Wall -Wextra -Werror -pedantic-errors)
target_include_directories(piejam_ladspa
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_ladspa
    PUBLIC piejam_audio
    PRIVATE fmt spdlog::spdlog piejam_range)
