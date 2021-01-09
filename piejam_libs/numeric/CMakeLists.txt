# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_numeric INTERFACE)

target_sources(piejam_numeric INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/bitops.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/endian.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/intops.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/intx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/limits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/numeric/type_traits.h
)

target_include_directories(piejam_numeric INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
