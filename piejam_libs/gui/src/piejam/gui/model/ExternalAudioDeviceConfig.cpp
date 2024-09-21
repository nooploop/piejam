// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/ExternalAudioDeviceConfig.h>

#include <piejam/runtime/actions/external_audio_device_actions.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

ExternalAudioDeviceConfig::ExternalAudioDeviceConfig(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::external_audio::device_id const device_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_device_id(device_id)
{
}

void
ExternalAudioDeviceConfig::onSubscribe()
{
    observe(runtime::selectors::make_external_audio_device_name_selector(
                    m_device_id),
            [this](std::string const& name) {
                setName(QString::fromStdString(name));
            });

    observe(runtime::selectors::make_external_audio_device_bus_type_selector(
                    m_device_id),
            [this](audio::bus_type const t) {
                setMono(t == audio::bus_type::mono);
            });

    observe(runtime::selectors::make_external_audio_device_bus_channel_selector(
                    m_device_id,
                    audio::bus_channel::mono),
            [this](std::size_t const ch) { setMonoChannel(ch + 1); });

    observe(runtime::selectors::make_external_audio_device_bus_channel_selector(
                    m_device_id,
                    audio::bus_channel::left),
            [this](std::size_t const ch) { setStereoLeftChannel(ch + 1); });

    observe(runtime::selectors::make_external_audio_device_bus_channel_selector(
                    m_device_id,
                    audio::bus_channel::right),
            [this](std::size_t const ch) { setStereoRightChannel(ch + 1); });
}

void
ExternalAudioDeviceConfig::changeName(QString const& name)
{
    runtime::actions::set_external_audio_device_name action;
    action.device_id = m_device_id;
    action.name = name.toStdString();
    dispatch(action);
}

static void
changeChannel(
        runtime::store_dispatch dispatch,
        runtime::external_audio::device_id const device_id,
        audio::bus_channel const bus_channel,
        unsigned const channel_index)
{
    runtime::actions::set_external_audio_device_bus_channel action;
    action.device_id = device_id;
    action.channel_selector = bus_channel;
    action.channel_index = static_cast<std::size_t>(channel_index) - 1;
    dispatch(action);
}

void
ExternalAudioDeviceConfig::changeMonoChannel(unsigned const ch)
{
    changeChannel(dispatch(), m_device_id, audio::bus_channel::mono, ch);
}

void
ExternalAudioDeviceConfig::changeStereoLeftChannel(unsigned const ch)
{
    changeChannel(dispatch(), m_device_id, audio::bus_channel::left, ch);
}

void
ExternalAudioDeviceConfig::changeStereoRightChannel(unsigned const ch)
{
    changeChannel(dispatch(), m_device_id, audio::bus_channel::right, ch);
}

void
ExternalAudioDeviceConfig::remove()
{
    runtime::actions::remove_external_audio_device action;
    action.device_id = m_device_id;
    dispatch(action);
}

} // namespace piejam::gui::model
