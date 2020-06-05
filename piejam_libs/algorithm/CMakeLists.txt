add_library(piejam_algorithm INTERFACE)

target_sources(piejam_algorithm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/copy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/find.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/find_if.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/find_or_get_first.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/index_of.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/set_intersection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/set_intersection_to_vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/string_join.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/transform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/algorithm/transform_to_vector.h
)

target_include_directories(piejam_algorithm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
