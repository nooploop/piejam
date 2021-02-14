# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_system STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/system/cpu_temp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/system/dll.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/system/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/system/device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/system/cpu_temp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/system/dll.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/system/device.cpp
)

target_include_directories(piejam_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(piejam_system PRIVATE -Wall -Wextra -Werror -pedantic-errors)

target_link_libraries(piejam_system PUBLIC stdc++fs PRIVATE dl)

add_subdirectory(tests)
