# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_range_test
    indices_test.cpp
    stride_pointer_iterator_test.cpp
    strided_span_test.cpp
    table_view_test.cpp
    zip_test.cpp
)

target_link_libraries(piejam_range_test gtest_driver piejam_range)
target_compile_options(piejam_range_test PRIVATE -Wall -Wextra -Werror -pedantic-errors -fconcepts-diagnostics-depth=2)

add_test(NAME piejam_range_test COMMAND piejam_range_test)

install(TARGETS piejam_range_test RUNTIME DESTINATION bin)
