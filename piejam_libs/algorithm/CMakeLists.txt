# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_algorithm INTERFACE)

target_sources(piejam_algorithm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/all_of_adjacent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/concat.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/concepts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/contains.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/edit_script.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/escape_html.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/find_or_get_first.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/for_each_adjacent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/for_each_visit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/index_of.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/shift_push_back.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/string_join.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/transform_to_vector.h
)

target_include_directories(piejam_algorithm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_algorithm INTERFACE piejam_base)

add_subdirectory(tests)
