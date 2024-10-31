# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_numeric_test
    bit_test.cpp
    intops_test.cpp
    mipp_iterator_test.cpp
)
target_link_libraries(piejam_numeric_test gtest_driver piejam_numeric)
target_compile_options(piejam_numeric_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_numeric_test COMMAND piejam_numeric_test)

install(TARGETS piejam_numeric_test RUNTIME DESTINATION bin)
