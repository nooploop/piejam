add_library(piejam_range INTERFACE)

target_sources(piejam_range INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/stride_iterator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/strided_span.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/range/table_view.h
)

target_include_directories(piejam_range INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
