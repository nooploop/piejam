if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_audio_test
    ${CMAKE_CURRENT_SOURCE_DIR}/dag_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph_to_dag_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/level_meter_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mix_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pan_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcm_convert_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/smoother_test.cpp
)
target_link_libraries(piejam_audio_test gtest gtest_main piejam_audio)
target_compile_options(piejam_audio_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_audio_test COMMAND piejam_audio_test)
