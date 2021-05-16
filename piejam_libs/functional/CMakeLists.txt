# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_functional INTERFACE)

target_sources(piejam_functional INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/address_compare.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/compare.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/deref.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/get.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/memo.h
)

target_include_directories(piejam_functional INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(piejam_functional INTERFACE piejam_base)

add_subdirectory(tests)
