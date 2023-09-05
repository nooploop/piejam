# SPDX-FileCopyrightText: 2023 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)

set(RESOUCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/piejam_setup_resources.qrc")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES ${RESOUCE_FILES})
else()
    qt5_add_resources(RESOURCES ${RESOUCE_FILES})
endif()

add_executable(piejam_setup
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MainModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Task.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TaskList.h
)
target_include_directories(piejam_setup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(piejam_setup PRIVATE -Wall -Wextra -Werror -Wno-error=deprecated-declarations -pedantic-errors)
target_link_libraries(piejam_setup Qt5::Quick Qt5::QuickControls2)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(piejam_setup PRIVATE QT_QML_DEBUG=1)
endif()
install(TARGETS piejam_setup RUNTIME DESTINATION bin)
