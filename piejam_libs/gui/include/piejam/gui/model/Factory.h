// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Factory.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class Factory final : public QObject
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
    Q_PROPERTY(piejam::gui::model::FxChain* fxChain READ fxChain CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxBrowser* fxBrowser READ fxBrowser CONSTANT)

public:
    Factory(runtime::store_dispatch, runtime::subscriber&);
    ~Factory();

    auto audioDeviceSettings() const -> piejam::gui::model::AudioDeviceSettings*
    {
        return m_audioDeviceSettings.get();
    }

    auto audioInputSettings() const
            -> piejam::gui::model::AudioInputOutputSettings*
    {
        return m_audioInputSettings.get();
    }

    auto audioOutputSettings() const
            -> piejam::gui::model::AudioInputOutputSettings*
    {
        return m_audioOutputSettings.get();
    }

    auto midiInputSettings() const -> piejam::gui::model::MidiInputSettings*
    {
        return m_midiInputSettings.get();
    }

    auto mixer() const -> piejam::gui::model::Mixer* { return m_mixer.get(); }

    auto info() const -> piejam::gui::model::Info* { return m_info.get(); }

    auto fxChain() const -> piejam::gui::model::FxChain*
    {
        return m_fxChain.get();
    }

    auto fxBrowser() const -> piejam::gui::model::FxBrowser*
    {
        return m_fxBrowser.get();
    }

private:
    runtime::store_dispatch m_store_dispatch;
    runtime::subscriber& m_state_change_subscriber;

    std::unique_ptr<piejam::gui::model::AudioDeviceSettings>
            m_audioDeviceSettings;
    std::unique_ptr<piejam::gui::model::AudioInputOutputSettings>
            m_audioInputSettings;
    std::unique_ptr<piejam::gui::model::AudioInputOutputSettings>
            m_audioOutputSettings;
    std::unique_ptr<piejam::gui::model::MidiInputSettings> m_midiInputSettings;
    std::unique_ptr<piejam::gui::model::Mixer> m_mixer;
    std::unique_ptr<piejam::gui::model::Info> m_info;
    std::unique_ptr<piejam::gui::model::FxChain> m_fxChain;
    std::unique_ptr<piejam::gui::model::FxBrowser> m_fxBrowser;
};

} // namespace piejam::gui::model
