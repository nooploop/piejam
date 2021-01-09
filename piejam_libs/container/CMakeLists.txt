# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_container INTERFACE)

target_sources(piejam_container INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/allocator_alignment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/table.h
)

target_include_directories(piejam_container
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
    SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/libs/xsimd/include)
target_link_libraries(piejam_container INTERFACE piejam_range)

add_subdirectory(tests)
