# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_functional INTERFACE)

target_sources(piejam_functional INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/address_compare.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/any_equal_to.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/edge_detect.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/get.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/get_if.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/in_interval.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/memo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/operators.h
)

target_include_directories(piejam_functional INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
