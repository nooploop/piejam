# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_thread STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/affinity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/cache_line_size.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/configuration.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/name.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/priority.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/spsc_slot.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/thread/affinity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/thread/configuration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/thread/name.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/thread/priority.cpp
)

target_include_directories(piejam_thread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(piejam_thread PRIVATE -Wall -Wextra -Werror -pedantic-errors)

target_link_libraries(piejam_thread PRIVATE pthread)

add_subdirectory(benchmarks)
add_subdirectory(tests)
