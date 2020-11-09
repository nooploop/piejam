add_library(piejam_thread STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/affinity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/configuration.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/job_deque.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/priority.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/thread/worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/thread/affinity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/thread/priority.cpp
)

target_include_directories(piejam_thread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(piejam_thread PRIVATE -Wall -Wextra -Werror -pedantic-errors)

target_link_libraries(piejam_thread PUBLIC piejam_base PRIVATE pthread)

add_subdirectory(tests)
