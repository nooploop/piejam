# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_audio_test
    clip_processor_test.cpp
    component_mock.h
    dag_test.cpp
    dsp_pitch_yin_test.cpp
    dsp_rms_test.cpp
    event_buffer_memory_test.cpp
    event_buffer_test.cpp
    event_converter_processor_test.cpp
    event_identity_processor_test.cpp
    event_input_buffers_test.cpp
    event_output_buffers_test.cpp
    event_test.cpp
    event_to_audio_processor_test.cpp
    fake_processor.h
    graph_algorithms_test.cpp
    graph_test.cpp
    graph_to_dag_test.cpp
    identity_processor_test.cpp
    input_processor_test.cpp
    io_process_test.cpp
    level_meter_processor_test.cpp
    lockstep_events_test.cpp
    mix_processor_test.cpp
    multichannel_buffer_test.cpp
    multiply_processor_test.cpp
    output_processor_test.cpp
    pan_balance_processor_test.cpp
    pan_component_test.cpp
    pan_test.cpp
    pcm_convert_test.cpp
    pitch_test.cpp
    process_test.cpp
    process_thread_test.cpp
    processor_mock.h
    slice_algorithms_test.cpp
    slice_test.cpp
    smoother_test.cpp
    stream_processor_test.cpp
    stream_ring_buffer_test.cpp
    value_io_processor_test.cpp
)
target_link_libraries(piejam_audio_test gtest_driver gmock piejam_audio piejam_range)
target_compile_options(piejam_audio_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_audio_test COMMAND piejam_audio_test)

install(TARGETS piejam_audio_test RUNTIME DESTINATION bin)
