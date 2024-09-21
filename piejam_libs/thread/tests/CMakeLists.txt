# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_thread_test
    ${CMAKE_CURRENT_SOURCE_DIR}/spsc_slot_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker_test.cpp
)
target_link_libraries(piejam_thread_test gtest_driver gmock piejam_thread)
target_compile_options(piejam_thread_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_thread_test COMMAND piejam_thread_test)

install(TARGETS piejam_thread_test RUNTIME DESTINATION bin)
