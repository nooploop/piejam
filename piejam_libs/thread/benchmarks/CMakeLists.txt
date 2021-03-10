# SPDX-FileCopyrightText: 2021 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_BENCHMARKS)
    return()
endif()

find_package(benchmark REQUIRED)

add_executable(piejam_thread_benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/spsc_slot_benchmark.cpp
)
target_link_libraries(piejam_thread_benchmark benchmark benchmark_main piejam_thread)
target_compile_options(piejam_thread_benchmark PRIVATE -Wall -Wextra -Werror -pedantic-errors)

install(TARGETS piejam_thread_benchmark RUNTIME DESTINATION bin)
