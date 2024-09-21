# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_library(piejam_system_test_dll SHARED ${CMAKE_CURRENT_SOURCE_DIR}/test_dll.c)

add_executable(piejam_system_test
    ${CMAKE_CURRENT_SOURCE_DIR}/dll_test.cpp
)
target_link_libraries(piejam_system_test gtest_driver piejam_system)
target_compile_options(piejam_system_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)
target_compile_definitions(piejam_system_test PRIVATE PIEJAM_SYSTEM_TEST_DLL="$<TARGET_FILE:piejam_system_test_dll>")

add_test(NAME piejam_system_test COMMAND piejam_system_test)

install(TARGETS piejam_system_test RUNTIME DESTINATION bin)
