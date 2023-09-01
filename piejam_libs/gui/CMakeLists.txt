# SPDX-FileCopyrightText: 2020 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Svg REQUIRED)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/item/Scope.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/item/Spectrum.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/item/Waveform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioDeviceSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioInputOutputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioStreamAmplifier.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioStreamChannelDuplicator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioStreamListener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/AudioStreamProvider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/BusConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/DbScaleData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/DbScaleTick.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/EnumListModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBoolParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBrowser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBrowserEntry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBrowserEntryInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxBrowserEntryLADSPA.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxChainModule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxEnumParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxFilter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxFloatParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxGenericModule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxIntParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxModule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxModuleContent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxModuleContentSubscribable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxParameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxParameterId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxScope.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxSpectrum.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/FxStreamKeyId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/GenericListModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Info.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MidiAssignable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MidiDeviceConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MidiInputSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Mixer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MixerChannel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MixerChannelEdit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MixerChannelFx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MixerChannelPerform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/MixerDbScales.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/RootView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/ScopeData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/ScopeDataGenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/SpectrumData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/SpectrumDataGenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/SpectrumDataPoint.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/StreamProcessor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/StringList.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Subscribable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/SubscribableModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/Types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/WaveformData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/WaveformDataGenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/WaveformDataObject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/model/fwd.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/piejam/gui/qt_log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/item/Scope.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/item/Spectrum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/item/Waveform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/AudioDeviceSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/AudioInputOutputSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/AudioStreamAmplifier.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/AudioStreamChannelDuplicator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/BusConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/DbScaleData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/Factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxBoolParameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxBrowserEntryInternal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxBrowserEntryLADSPA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxChainModule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxEnumParameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxFloatParameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxGenericModule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxIntParameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxModule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxParameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxScope.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxSpectrum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/FxStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/Info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MidiAssignable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MidiDeviceConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MidiInputSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/Mixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MixerChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MixerChannelEdit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MixerChannelFx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MixerChannelPerform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/MixerDbScales.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/RootView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/ScopeDataGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/SpectrumDataGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/model/WaveformDataGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/piejam/gui/qt_log.cpp
)
target_include_directories(piejam_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(piejam_gui
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    Qt5::VirtualKeyboard
    fmt
    spdlog::spdlog
    piejam_base
    piejam_range
    piejam_algorithm
    piejam_log
    piejam_numeric
    piejam_runtime)
target_compile_options(piejam_gui PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_subdirectory(tests)
