# SPDX-FileCopyrightText: 2021 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_library(piejam_midi STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/midi/event.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/midi/event_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/midi/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/midi/device_id.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/midi/manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/midi/alsa.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/midi/alsa.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/midi/manager.cpp
)

target_compile_options(piejam_midi PRIVATE -Wall -Wextra -Werror -pedantic-errors)
target_include_directories(piejam_midi
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_midi
    PUBLIC piejam_algorithm piejam_system
    PRIVATE fmt spdlog::spdlog)
