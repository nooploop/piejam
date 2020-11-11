add_library(piejam_runtime STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/apply_app_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/device_action.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/device_action_visitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/device_actions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/engine_action.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/engine_action_visitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/engine_actions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/actions/set_bus_name.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/app_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/audio_components/mixer_bus_processor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/audio_components/mute_solo_processor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/audio_engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/audio_engine_middleware.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/audio_state.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/audio_state_selectors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/mixer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/open_alsa_device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/selected_device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/runtime/ui/action.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/actions/apply_app_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/actions/device_actions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/actions/engine_actions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/actions/set_bus_name.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/app_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/audio_components/mixer_bus_processor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/audio_components/mute_solo_processor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/audio_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/audio_engine_middleware.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/audio_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/audio_state_selectors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/runtime/open_alsa_device.cpp
)

find_package(nlohmann_json REQUIRED)

target_compile_options(piejam_runtime PRIVATE -Wall -Wextra -Werror -pedantic-errors)
target_include_directories(piejam_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_runtime
    PUBLIC
    piejam_redux
    piejam_numeric
    piejam_thread
    piejam_audio
    piejam_functional
    piejam_container
    piejam_reselect
    nlohmann_json
    PRIVATE
    fmt
    spdlog::spdlog
    piejam_algorithm
    piejam_range)

add_subdirectory(tests)
