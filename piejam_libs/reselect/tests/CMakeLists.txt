if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_reselect_test
    ${CMAKE_CURRENT_SOURCE_DIR}/selector_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subscriber_test.cpp
)
target_link_libraries(piejam_reselect_test gtest gtest_main piejam_reselect)
target_compile_options(piejam_reselect_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_reselect_test COMMAND piejam_reselect_test)
