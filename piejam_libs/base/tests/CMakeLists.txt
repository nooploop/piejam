if(NOT PIEJAM_TESTS)
    return()
endif()

find_package(GTest REQUIRED)
add_executable(piejam_base_test
    ${CMAKE_CURRENT_SOURCE_DIR}/box_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/boxify_result_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/decay_tuple_elements_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity_map_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/type_traits_test.cpp
)
target_link_libraries(piejam_base_test gtest gtest_main piejam_base)
target_compile_options(piejam_base_test PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_test(NAME piejam_base_test COMMAND piejam_base_test)
