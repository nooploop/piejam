# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_log INTERFACE)

target_sources(piejam_log INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/log/generic_log_sink.h
)

target_include_directories(piejam_log INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(piejam_log INTERFACE spdlog::spdlog)
