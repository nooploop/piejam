add_library(piejam_redux INTERFACE)

target_sources(piejam_redux INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/functors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/store.h
)

target_include_directories(piejam_redux INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
