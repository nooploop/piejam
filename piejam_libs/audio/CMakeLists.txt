add_library(piejam_audio STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/alsa/get_pcm_io_descriptors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/alsa/get_set_hw_params.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/alsa/pcm_io.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/alsa/pcm_reader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/alsa/pcm_writer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/alsa/process_step.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/cpu_load_meter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/level_meter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/mixer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pair.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pan.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pcm_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pcm_descriptor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pcm_format.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pcm_hw_params.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pcm_io_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/pcm_sample_type.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/period_sizes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/process_function.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/process_thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/samplerates.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/audio/smoother.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/audio/alsa/get_pcm_io_descriptors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/audio/alsa/get_set_hw_params.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/audio/alsa/pcm_io.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/audio/alsa/process_step.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/audio/cpu_load_meter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/audio/engine.cpp
)

target_include_directories(piejam_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_audio
    PUBLIC piejam_io piejam_numeric piejam_thread
    PRIVATE fmt spdlog::spdlog piejam_algorithm piejam_container piejam_range)

add_subdirectory(test)
