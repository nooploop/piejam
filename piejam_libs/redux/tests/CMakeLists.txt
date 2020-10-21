if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_redux_test
    ${CMAKE_CURRENT_SOURCE_DIR}/store_test.cpp
)
target_link_libraries(piejam_redux_test gtest gtest_main piejam_redux)
target_compile_options(piejam_redux_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_redux_test COMMAND piejam_redux_test)
