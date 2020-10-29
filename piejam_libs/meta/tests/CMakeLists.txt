if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_meta_test
    ${CMAKE_CURRENT_SOURCE_DIR}/decay_tuple_elements_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/type_traits_test.cpp
)
target_link_libraries(piejam_meta_test gtest gtest_main piejam_meta)
target_compile_options(piejam_meta_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_meta_test COMMAND piejam_meta_test)
