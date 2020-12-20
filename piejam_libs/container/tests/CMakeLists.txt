if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_container_test
    ${CMAKE_CURRENT_SOURCE_DIR}/table_test.cpp
)
target_link_libraries(piejam_container_test gtest gtest_main piejam_container)
target_compile_options(piejam_container_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_container_test COMMAND piejam_container_test)
