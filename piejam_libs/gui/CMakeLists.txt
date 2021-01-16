# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5VirtualKeyboard REQUIRED)

set(RESOUCE_FILES "resources/piejam_gui_resources.qrc")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES ${RESOUCE_FILES})
else()
    qt5_add_resources(RESOURCES ${RESOUCE_FILES})
endif()

list(APPEND PIEJAM_QML_IMPORT_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(PIEJAM_QML_IMPORT_PATHS ${PIEJAM_QML_IMPORT_PATHS} PARENT_SCOPE)

add_library(piejam_gui STATIC
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/generic_list_model_edit_script_executor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioDeviceSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioInputOutputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/BusConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/BusName.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBrowserEntry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxChain.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxModule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/GenericListModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Info.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MidiDeviceConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MidiInputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Mixer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MixerChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/StringList.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/SubscribableModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/qt_log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/AudioInputOutputSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/BusConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/Info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/Mixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MixerChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/StringList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/qt_log.cpp
)
target_include_directories(piejam_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_gui Qt5::Quick Qt5::QuickControls2 Qt5::VirtualKeyboard fmt spdlog::spdlog piejam_base)
target_compile_options(piejam_gui PRIVATE -Wall -Wextra -Werror -pedantic-errors)
