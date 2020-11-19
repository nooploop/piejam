if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_runtime_test
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_middleware_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mixer_bus_processor_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parameter_map_test.cpp
)
target_link_libraries(piejam_runtime_test gtest gtest_main gmock piejam_runtime)
target_compile_options(piejam_runtime_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_runtime_test COMMAND piejam_runtime_test)


add_executable(piejam_runtime_render_test
    ${CMAKE_CURRENT_SOURCE_DIR}/audio_engine_render_test.cpp)
target_link_libraries(piejam_runtime_render_test gtest gtest_main piejam_runtime)
target_compile_options(piejam_runtime_render_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)
