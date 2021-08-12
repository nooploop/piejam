// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/BusConfig.h>

#include <piejam/runtime/actions/delete_bus.h>
#include <piejam/runtime/actions/select_bus_channel.h>
#include <piejam/runtime/actions/set_device_bus_name.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

BusConfig::BusConfig(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::device_io::bus_id bus_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_bus_id(bus_id)
{
}

void
BusConfig::onSubscribe()
{
    observe(runtime::selectors::make_device_bus_name_selector(m_bus_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_bus_type_selector(m_bus_id),
            [this](audio::bus_type const t) {
                setMono(t == audio::bus_type::mono);
            });

    observe(runtime::selectors::make_bus_channel_selector(
                    m_bus_id,
                    audio::bus_channel::mono),
            [this](std::size_t const ch) { setMonoChannel(ch + 1); });

    observe(runtime::selectors::make_bus_channel_selector(
                    m_bus_id,
                    audio::bus_channel::left),
            [this](std::size_t const ch) { setStereoLeftChannel(ch + 1); });

    observe(runtime::selectors::make_bus_channel_selector(
                    m_bus_id,
                    audio::bus_channel::right),
            [this](std::size_t const ch) { setStereoRightChannel(ch + 1); });
}

void
BusConfig::changeName(QString const& name)
{
    runtime::actions::set_device_bus_name action;
    action.bus_id = m_bus_id;
    action.name = name.toStdString();
    dispatch(action);
}

static void
changeChannel(
        runtime::store_dispatch dispatch,
        runtime::device_io::bus_id bus_id,
        audio::bus_channel const bus_channel,
        unsigned const channel_index)
{
    runtime::actions::select_bus_channel action;
    action.bus_id = bus_id;
    action.channel_selector = bus_channel;
    action.channel_index = static_cast<std::size_t>(channel_index) - 1;
    dispatch(action);
}

void
BusConfig::changeMonoChannel(unsigned const ch)
{
    changeChannel(dispatch(), m_bus_id, audio::bus_channel::mono, ch);
}

void
BusConfig::changeStereoLeftChannel(unsigned const ch)
{
    changeChannel(dispatch(), m_bus_id, audio::bus_channel::left, ch);
}

void
BusConfig::changeStereoRightChannel(unsigned const ch)
{
    changeChannel(dispatch(), m_bus_id, audio::bus_channel::right, ch);
}

void
BusConfig::deleteBus()
{
    runtime::actions::delete_bus action;
    action.bus_id = m_bus_id;
    dispatch(action);
}

} // namespace piejam::gui::model
