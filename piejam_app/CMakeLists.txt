# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core)

set(RESOUCE_FILES "resources/piejam_app_resources.qrc")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES ${RESOUCE_FILES})
else()
    qt5_add_resources(RESOURCES ${RESOUCE_FILES})
endif()

add_executable(piejam_app
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioDeviceSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioDeviceSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioInputOutputSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/AudioInputOutputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/BusConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/BusConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxBrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxBrowserEntryInternal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxBrowserEntryInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxBrowserEntryLADSPA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxBrowserEntryLADSPA.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxChain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxChain.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxGenericModule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxGenericModule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxModule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxModule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxParameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxParameterKeyId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxScope.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxScope.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxSpectrum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxSpectrum.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/FxStreamKeyId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Info.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MidiAssignable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MidiAssignable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MidiDeviceConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MidiDeviceConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MidiInputSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MidiInputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Mixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Mixer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MixerChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MixerChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MixerChannelEdit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MixerChannelEdit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MixerChannelPerform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/MixerChannelPerform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/app/gui/model/Subscribable.h
)
target_include_directories(piejam_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(piejam_app PRIVATE piejam_runtime piejam_redux piejam_algorithm piejam_log piejam_gui)
target_compile_options(piejam_app PRIVATE -Wall -Wextra -Werror -Wno-error=deprecated-declarations -pedantic-errors)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(piejam_app PRIVATE QT_QML_DEBUG=1)
endif()
install(TARGETS piejam_app RUNTIME DESTINATION bin)
