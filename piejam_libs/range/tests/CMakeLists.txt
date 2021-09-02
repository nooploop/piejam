# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_range_test
    ${CMAKE_CURRENT_SOURCE_DIR}/frame_iterator_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/indices_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interleaved_vector_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/interleaved_view_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stride_iterator_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/strided_span_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table_view_test.cpp
)
target_link_libraries(piejam_range_test gtest_driver piejam_range)
target_compile_options(piejam_range_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_range_test COMMAND piejam_range_test)

install(TARGETS piejam_range_test RUNTIME DESTINATION bin)
