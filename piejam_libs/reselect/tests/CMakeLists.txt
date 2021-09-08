# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

if(NOT PIEJAM_TESTS)
    return()
endif()

add_executable(piejam_reselect_test
    ${CMAKE_CURRENT_SOURCE_DIR}/selector_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subscriber_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subscriptions_manager_test.cpp
)
target_link_libraries(piejam_reselect_test gtest_driver piejam_reselect)
target_compile_options(piejam_reselect_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_reselect_test COMMAND piejam_reselect_test)

install(TARGETS piejam_reselect_test RUNTIME DESTINATION bin)
