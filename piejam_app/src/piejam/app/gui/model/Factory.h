// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Factory.h>

#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

#include <memory>

namespace piejam::app::gui::model
{

class Factory final : public piejam::gui::model::Factory
{
    Q_OBJECT

public:
    Factory(runtime::store_dispatch, runtime::subscriber&);
    ~Factory();

    auto audioDeviceSettings() const
            -> piejam::gui::model::AudioDeviceSettings* override
    {
        return m_audioDeviceSettings.get();
    }

    auto audioInputSettings() const
            -> piejam::gui::model::AudioInputOutputSettings* override
    {
        return m_audioInputSettings.get();
    }

    auto audioOutputSettings() const
            -> piejam::gui::model::AudioInputOutputSettings* override
    {
        return m_audioOutputSettings.get();
    }

    auto mixer() const -> piejam::gui::model::Mixer* override
    {
        return m_mixer.get();
    }

    auto info() const -> piejam::gui::model::Info* override
    {
        return m_info.get();
    }

    auto fxChain() const -> piejam::gui::model::FxChain* override
    {
        return m_fxChain.get();
    }

    auto fxBrowser() const -> piejam::gui::model::FxBrowser* override
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
    std::unique_ptr<piejam::gui::model::Mixer> m_mixer;
    std::unique_ptr<piejam::gui::model::Info> m_info;
    std::unique_ptr<piejam::gui::model::FxChain> m_fxChain;
    std::unique_ptr<piejam::gui::model::FxBrowser> m_fxBrowser;
};

} // namespace piejam::app::gui::model
