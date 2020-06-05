add_library(piejam_meta INTERFACE)

target_sources(piejam_meta INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/meta/decay_tuple_elements.h
)

target_include_directories(piejam_meta INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
