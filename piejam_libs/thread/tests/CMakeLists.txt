if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_thread_test
    ${CMAKE_CURRENT_SOURCE_DIR}/job_deque_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker_test.cpp
)
target_link_libraries(piejam_thread_test gtest gtest_main gmock piejam_thread)
target_compile_options(piejam_thread_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_thread_test COMMAND piejam_thread_test)
