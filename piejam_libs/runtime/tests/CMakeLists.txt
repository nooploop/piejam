# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_runtime_test
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_device_manager_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_engine_middleware_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_key_object_map_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fx_parameter_id_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ladspa_fx_middleware_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ladspa_instance_manager_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ladspa_processor_factory_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/middleware_functors_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/midi_assignment_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/midi_control_middleware_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/midi_input_controller_mock.h
    ${CMAKE_CURRENT_SOURCE_DIR}/midi_input_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/midi_learn_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/midi_to_parameter_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mute_solo_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mixer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parameter_map_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parameter_processor_factory_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stream_processor_factory_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/state_test.cpp
)
target_link_libraries(piejam_runtime_test gtest_driver gmock piejam_runtime)
target_compile_options(piejam_runtime_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_runtime_test COMMAND piejam_runtime_test)

install(TARGETS piejam_runtime_test RUNTIME DESTINATION bin)

add_executable(piejam_runtime_render_test
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_engine_render_test.cpp)
target_link_libraries(piejam_runtime_render_test gtest_driver piejam_runtime)
target_compile_options(piejam_runtime_render_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)
