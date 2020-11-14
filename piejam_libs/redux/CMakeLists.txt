add_library(piejam_redux INTERFACE)

target_sources(piejam_redux INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/cloneable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/functors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/queueing_middleware.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/store.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/thread_delegate_middleware.h
)

target_include_directories(piejam_redux INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
