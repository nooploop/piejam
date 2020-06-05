add_library(piejam_reselect INTERFACE)

target_sources(piejam_reselect INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/selector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/subscriber.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/subscription.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/subscriptions_manager.h
)

target_include_directories(piejam_reselect INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(piejam_reselect SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/libs/nod/include)

add_subdirectory(test)
