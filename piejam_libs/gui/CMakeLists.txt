# SPDX-FileCopyrightText: 2020-2024 Dimitrij Kotrev
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5VirtualKeyboard REQUIRED)

set(RESOURCE_FILES "resources/piejam_gui_resources.qrc")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES ${RESOURCE_FILES})
else()
    qt5_add_resources(RESOURCES ${RESOURCE_FILES})
endif()

list(APPEND PIEJAM_QML_IMPORT_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(PIEJAM_QML_IMPORT_PATHS ${PIEJAM_QML_IMPORT_PATHS} PARENT_SCOPE)

add_library(piejam_gui STATIC
    ${RESOURCES}
    include/piejam/gui/ModelManager.h
    include/piejam/gui/PropertyMacros.h
    include/piejam/gui/generic_list_model_edit_script_executor.h
    include/piejam/gui/init.h
    include/piejam/gui/item/DbScale.h
    include/piejam/gui/item/Scope.h
    include/piejam/gui/item/Spectrum.h
    include/piejam/gui/item/Waveform.h
    include/piejam/gui/model/AudioDeviceSettings.h
    include/piejam/gui/model/AudioInputOutputSettings.h
    include/piejam/gui/model/AudioRouting.h
    include/piejam/gui/model/AudioRoutingSelection.h
    include/piejam/gui/model/AudioStream.h
    include/piejam/gui/model/AudioStreamAmplifier.h
    include/piejam/gui/model/AudioStreamChannelDuplicator.h
    include/piejam/gui/model/AudioStreamChannelSplitter.h
    include/piejam/gui/model/AudioStreamListener.h
    include/piejam/gui/model/AudioStreamProvider.h
    include/piejam/gui/model/BoolParameter.h
    include/piejam/gui/model/DbScaleData.h
    include/piejam/gui/model/DbScaleTick.h
    include/piejam/gui/model/EnumListModel.h
    include/piejam/gui/model/EnumParameter.h
    include/piejam/gui/model/ExternalAudioDeviceConfig.h
    include/piejam/gui/model/FloatParameter.h
    include/piejam/gui/model/FxBrowser.h
    include/piejam/gui/model/FxBrowserEntry.h
    include/piejam/gui/model/FxBrowserEntryInternal.h
    include/piejam/gui/model/FxBrowserEntryInternalData.h
    include/piejam/gui/model/FxBrowserEntryLADSPA.h
    include/piejam/gui/model/FxChainModule.h
    include/piejam/gui/model/FxGenericModule.h
    include/piejam/gui/model/FxModule.h
    include/piejam/gui/model/FxModuleFactory.h
    include/piejam/gui/model/FxModuleRegistry.h
    include/piejam/gui/model/FxModuleType.h
    include/piejam/gui/model/FxModuleView.h
    include/piejam/gui/model/FxStream.h
    include/piejam/gui/model/GenericListModel.h
    include/piejam/gui/model/Info.h
    include/piejam/gui/model/IntParameter.h
    include/piejam/gui/model/Log.h
    include/piejam/gui/model/MidiAssignable.h
    include/piejam/gui/model/MidiDeviceConfig.h
    include/piejam/gui/model/MidiInputSettings.h
    include/piejam/gui/model/Mixer.h
    include/piejam/gui/model/MixerChannel.h
    include/piejam/gui/model/MixerChannelAuxSend.h
    include/piejam/gui/model/MixerChannelEdit.h
    include/piejam/gui/model/MixerChannelFx.h
    include/piejam/gui/model/MixerChannelModels.h
    include/piejam/gui/model/MixerChannelPerform.h
    include/piejam/gui/model/MixerDbScales.h
    include/piejam/gui/model/Parameter.h
    include/piejam/gui/model/ParameterId.h
    include/piejam/gui/model/PeakLevelMeter.h
    include/piejam/gui/model/PitchGenerator.h
    include/piejam/gui/model/RmsLevelMeter.h
    include/piejam/gui/model/RootView.h
    include/piejam/gui/model/ScopeData.h
    include/piejam/gui/model/ScopeDataGenerator.h
    include/piejam/gui/model/SpectrumData.h
    include/piejam/gui/model/SpectrumDataGenerator.h
    include/piejam/gui/model/SpectrumDataPoint.h
    include/piejam/gui/model/StereoLevel.h
    include/piejam/gui/model/StreamId.h
    include/piejam/gui/model/StreamProcessor.h
    include/piejam/gui/model/StringList.h
    include/piejam/gui/model/Subscribable.h
    include/piejam/gui/model/SubscribableModel.h
    include/piejam/gui/model/Types.h
    include/piejam/gui/model/WaveformData.h
    include/piejam/gui/model/WaveformDataGenerator.h
    include/piejam/gui/model/WaveformDataObject.h
    include/piejam/gui/model/fwd.h
    include/piejam/gui/qt_log.h
    src/piejam/gui/ModelManager.cpp
    src/piejam/gui/init.cpp
    src/piejam/gui/item/DbScale.cpp
    src/piejam/gui/item/Scope.cpp
    src/piejam/gui/item/Spectrum.cpp
    src/piejam/gui/item/Waveform.cpp
    src/piejam/gui/model/AudioDeviceSettings.cpp
    src/piejam/gui/model/AudioInputOutputSettings.cpp
    src/piejam/gui/model/AudioRouting.cpp
    src/piejam/gui/model/AudioRoutingSelection.cpp
    src/piejam/gui/model/AudioStreamAmplifier.cpp
    src/piejam/gui/model/AudioStreamChannelDuplicator.cpp
    src/piejam/gui/model/AudioStreamChannelSplitter.cpp
    src/piejam/gui/model/BoolParameter.cpp
    src/piejam/gui/model/DbScaleData.cpp
    src/piejam/gui/model/EnumParameter.cpp
    src/piejam/gui/model/ExternalAudioDeviceConfig.cpp
    src/piejam/gui/model/FloatParameter.cpp
    src/piejam/gui/model/FxBrowser.cpp
    src/piejam/gui/model/FxBrowserEntryInternal.cpp
    src/piejam/gui/model/FxBrowserEntryLADSPA.cpp
    src/piejam/gui/model/FxChainModule.cpp
    src/piejam/gui/model/FxGenericModule.cpp
    src/piejam/gui/model/FxModule.cpp
    src/piejam/gui/model/FxModuleRegistry.cpp
    src/piejam/gui/model/FxModuleView.cpp
    src/piejam/gui/model/FxStream.cpp
    src/piejam/gui/model/Info.cpp
    src/piejam/gui/model/IntParameter.cpp
    src/piejam/gui/model/Log.cpp
    src/piejam/gui/model/MidiAssignable.cpp
    src/piejam/gui/model/MidiDeviceConfig.cpp
    src/piejam/gui/model/MidiInputSettings.cpp
    src/piejam/gui/model/Mixer.cpp
    src/piejam/gui/model/MixerChannel.cpp
    src/piejam/gui/model/MixerChannelAuxSend.cpp
    src/piejam/gui/model/MixerChannelEdit.cpp
    src/piejam/gui/model/MixerChannelFx.cpp
    src/piejam/gui/model/MixerChannelModels.cpp
    src/piejam/gui/model/MixerChannelPerform.cpp
    src/piejam/gui/model/MixerDbScales.cpp
    src/piejam/gui/model/Parameter.cpp
    src/piejam/gui/model/PeakLevelMeter.cpp
    src/piejam/gui/model/PitchGenerator.cpp
    src/piejam/gui/model/RmsLevelMeter.cpp
    src/piejam/gui/model/RootView.cpp
    src/piejam/gui/model/ScopeDataGenerator.cpp
    src/piejam/gui/model/SpectrumDataGenerator.cpp
    src/piejam/gui/model/WaveformDataGenerator.cpp
    src/piejam/gui/qt_log.cpp
    include/piejam/gui/item/FixedLinearScaleGrid.h
    src/piejam/gui/item/FixedLinearScaleGrid.cpp
    include/piejam/gui/item/FixedLogScaleGrid.h
    src/piejam/gui/item/FixedLogScaleGrid.cpp
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
    piejam_runtime
)
target_compile_options(piejam_gui PRIVATE -Wall -Wextra -Werror -pedantic-errors)

add_subdirectory(tests)

unset(RESOURCE_FILES)
unset(RESOURCES)
