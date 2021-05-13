# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_algorithm_test
    ${CMAKE_CURRENT_SOURCE_DIR}/contains_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/edit_script_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/index_of_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transform_to_vector_test.cpp
)
target_link_libraries(piejam_algorithm_test gtest gtest_main piejam_algorithm)
target_compile_options(piejam_algorithm_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_algorithm_test COMMAND piejam_algorithm_test)
