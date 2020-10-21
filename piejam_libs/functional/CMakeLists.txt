add_library(piejam_functional INTERFACE)

target_sources(piejam_functional INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/memo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/functional/overload.h
)

target_include_directories(piejam_functional INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(piejam_functional INTERFACE piejam_meta)

add_subdirectory(tests)
