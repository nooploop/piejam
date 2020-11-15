add_library(piejam_base INTERFACE)

target_sources(piejam_base INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/entity_map.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/math.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/npos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/tuple.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/type_traits.h
)

target_include_directories(piejam_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
