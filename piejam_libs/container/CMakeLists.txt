add_library(piejam_container INTERFACE)

target_sources(piejam_container INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/allocator_alignment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/boxed_string.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/boxify_result.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/container/table.h
)

target_include_directories(piejam_container INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_container INTERFACE piejam_range)

add_subdirectory(tests)
