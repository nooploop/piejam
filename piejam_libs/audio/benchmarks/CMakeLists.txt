if(NOT PIEJAM_BENCHMARKS)
    return()
endif()

find_package(benchmark REQUIRED)

add_executable(piejam_audio_benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/mix_benchmark.cpp
)
target_link_libraries(piejam_audio_benchmark benchmark benchmark_main piejam_audio)
target_compile_options(piejam_audio_benchmark PRIVATE -Wall -Wextra -Werror -pedantic-errors)
