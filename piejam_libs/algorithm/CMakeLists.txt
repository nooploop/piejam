add_library(piejam_algorithm INTERFACE)

target_sources(piejam_algorithm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/concat.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/contains.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/edit_script.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/escape_html.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/find_or_get_first.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/for_each_adjacent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/index_of.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/set_intersection_to_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/string_join.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/transform_to_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/unique_erase.h
)

target_include_directories(piejam_algorithm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_algorithm INTERFACE piejam_base)

add_subdirectory(tests)
