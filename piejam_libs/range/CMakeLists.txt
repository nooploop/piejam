# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_range INTERFACE)

target_sources(piejam_range INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/dynamic_stride.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/indices.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/iota.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/pointer_stride_iterator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/strided_span.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/table_view.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/zip.h
)

target_include_directories(piejam_range INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
