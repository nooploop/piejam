# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_functional_test
    ${CMAKE_CURRENT_SOURCE_DIR}/compare_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deref_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/get_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memo_test.cpp
)
target_link_libraries(piejam_functional_test gtest gtest_main piejam_functional)
target_compile_options(piejam_functional_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_functional_test COMMAND piejam_functional_test)
