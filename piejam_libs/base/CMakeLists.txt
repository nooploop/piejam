add_library(piejam_base INTERFACE)

target_sources(piejam_base INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/math.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/npos.h
)

target_include_directories(piejam_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
