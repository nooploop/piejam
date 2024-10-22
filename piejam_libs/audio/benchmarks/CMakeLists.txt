# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_BENCHMARKS)
    return()
endif()

find_package(benchmark REQUIRED)

add_executable(piejam_audio_benchmark
    mix_benchmark.cpp
    mix_processor_benchmark.cpp
    multiply_processor_benchmark.cpp
    peak_level_meter_benchmark.cpp
    pitch_yin_benchmark.cpp
    rms_benchmark.cpp
)
target_link_libraries(piejam_audio_benchmark benchmark benchmark_main piejam_audio)
target_compile_options(piejam_audio_benchmark PRIVATE -Wall -Wextra -Werror -pedantic-errors)

install(TARGETS piejam_audio_benchmark RUNTIME DESTINATION bin)
