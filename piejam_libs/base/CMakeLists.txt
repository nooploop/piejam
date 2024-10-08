# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_base INTERFACE)

target_sources(piejam_base INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/bool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/boxed_string.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/boxed_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/entity_data_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/entity_id.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/entity_id_hash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/entity_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/indexed_access.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/integral_constant.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/io_direction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/io_pair.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/math.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/npos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/on_scope_exit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/pimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/registry_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/set_if.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/switch_cast.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/to_underlying.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/tuple.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/tuple_element_compare.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/type_traits.h
)

target_include_directories(piejam_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
