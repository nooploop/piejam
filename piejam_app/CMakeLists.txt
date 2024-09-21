# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

add_executable(piejam_app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
target_include_directories(piejam_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(piejam_app PRIVATE piejam_runtime piejam_redux piejam_algorithm piejam_log piejam_gui piejam_fx_modules)
target_compile_options(piejam_app PRIVATE -Wall -Wextra -Werror -Wno-error=deprecated-declarations -pedantic-errors)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(piejam_app PRIVATE QT_QML_DEBUG=1)
endif()
install(TARGETS piejam_app RUNTIME DESTINATION bin)
