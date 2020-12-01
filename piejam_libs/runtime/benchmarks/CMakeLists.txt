if(NOT PIEJAM_BENCHMARKS)
    return()
endif()

find_package(benchmark REQUIRED)

add_executable(piejam_runtime_benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/state_benchmark.cpp
)
target_link_libraries(piejam_runtime_benchmark benchmark benchmark_main piejam_runtime)
target_compile_options(piejam_runtime_benchmark PRIVATE -Wall -Wextra -Werror -pedantic-errors)

install(TARGETS piejam_runtime_benchmark RUNTIME DESTINATION bin)
