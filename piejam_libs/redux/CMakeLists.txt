# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_redux INTERFACE)

target_sources(piejam_redux INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/action_tracker_middleware.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/concepts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/functors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/middleware_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/middleware_functors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/queueing_middleware.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/store.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/thread_delegate_middleware.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/redux/thunk_middleware.h
)

target_include_directories(piejam_redux INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests)
