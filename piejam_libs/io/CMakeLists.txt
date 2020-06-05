add_library(piejam_io STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/io/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/io/ioctl_device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/io/ioctl_device.cpp
)

target_include_directories(piejam_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(piejam_io PUBLIC stdc++fs)
