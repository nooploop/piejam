# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_audio_test
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_spsc_ring_buffer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dag_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_buffer_memory_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_buffer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_converter_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_identity_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_input_buffers_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_output_buffers_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/event_to_audio_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph_algorithms_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph_to_dag_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/level_meter_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/level_meter_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lockstep_events_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mix_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/output_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pan_component_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pan_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcm_convert_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/process_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/process_thread_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processor_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/slice_algorithms_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/smoother_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stream_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/value_input_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/value_output_processor_test.cpp
)
target_link_libraries(piejam_audio_test gtest_driver gmock piejam_audio piejam_range)
target_compile_options(piejam_audio_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_audio_test COMMAND piejam_audio_test)

install(TARGETS piejam_audio_test RUNTIME DESTINATION bin)
