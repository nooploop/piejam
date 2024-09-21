// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

#include <QObject>

#include <memory>

namespace piejam::gui
{

class ModelManager final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::AudioDeviceSettings* audioDeviceSettings READ
                       audioDeviceSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::AudioInputOutputSettings* audioInputSettings
                       READ audioInputSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::AudioInputOutputSettings* audioOutputSettings
                       READ audioOutputSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::MidiInputSettings* midiInputSettings READ
                       midiInputSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::Mixer* mixer READ mixer CONSTANT)
    Q_PROPERTY(piejam::gui::model::Info* info READ info CONSTANT)
    Q_PROPERTY(piejam::gui::model::Log* log READ log CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxBrowser* fxBrowser READ fxBrowser CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxModule* fxModule READ fxModule CONSTANT)

    Q_PROPERTY(piejam::gui::model::RootView* rootView READ rootView CONSTANT)

public:
    ModelManager(runtime::store_dispatch, runtime::subscriber&);
    ~ModelManager();

    auto audioDeviceSettings() const -> model::AudioDeviceSettings*
    {
        return m_audioDeviceSettings.get();
    }

    auto audioInputSettings() const -> model::AudioInputOutputSettings*
    {
        return m_audioInputSettings.get();
    }

    auto audioOutputSettings() const -> model::AudioInputOutputSettings*
    {
        return m_audioOutputSettings.get();
    }

    auto midiInputSettings() const -> model::MidiInputSettings*
    {
        return m_midiInputSettings.get();
    }

    auto mixer() const -> model::Mixer*
    {
        return m_mixer.get();
    }

    auto info() const -> model::Info*
    {
        return m_info.get();
    }

    auto log() const -> model::Log*
    {
        return m_log.get();
    }

    auto fxBrowser() const -> model::FxBrowser*
    {
        return m_fxBrowser.get();
    }

    auto fxModule() const -> model::FxModule*
    {
        return m_fxModule.get();
    }

    auto rootView() const -> model::RootView*
    {
        return m_rootView.get();
    }

private:
    runtime::store_dispatch m_store_dispatch;
    runtime::subscriber& m_state_change_subscriber;

    std::unique_ptr<model::AudioDeviceSettings> m_audioDeviceSettings;
    std::unique_ptr<model::AudioInputOutputSettings> m_audioInputSettings;
    std::unique_ptr<model::AudioInputOutputSettings> m_audioOutputSettings;
    std::unique_ptr<model::MidiInputSettings> m_midiInputSettings;
    std::unique_ptr<model::Mixer> m_mixer;
    std::unique_ptr<model::Info> m_info;
    std::unique_ptr<model::Log> m_log;
    std::unique_ptr<model::FxBrowser> m_fxBrowser;
    std::unique_ptr<model::FxModule> m_fxModule;

    std::unique_ptr<model::RootView> m_rootView;
};

} // namespace piejam::gui
