# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_base_test
    box_test.cpp
    decay_tuple_elements_test.cpp
    entity_map_test.cpp
    indexed_access_test.cpp
    math_test.cpp
    tuple_test.cpp
    type_traits_test.cpp
    entity_data_map_test.cpp
)
target_link_libraries(piejam_base_test gtest_driver piejam_base)
target_compile_options(piejam_base_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_base_test COMMAND piejam_base_test)

install(TARGETS piejam_base_test RUNTIME DESTINATION bin)
