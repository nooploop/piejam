# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_reselect INTERFACE)

target_sources(piejam_reselect INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/selector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/subscriber.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/subscription.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/reselect/subscriptions_manager.h
)

target_include_directories(piejam_reselect INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_reselect INTERFACE nod)

add_subdirectory(tests)
