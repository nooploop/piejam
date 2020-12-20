// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/app/gui/model/BusConfig.h>

#include <piejam/runtime/actions/delete_bus.h>
#include <piejam/runtime/actions/select_bus_channel.h>
#include <piejam/runtime/actions/set_bus_name.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

BusConfig::BusConfig(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::bus_id bus_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_bus_id(bus_id)
{
}

void
BusConfig::onSubscribe()
{
    observe(runtime::selectors::make_bus_name_selector(m_bus_id),
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
    runtime::actions::set_bus_name action;
    action.bus_id = m_bus_id;
    action.name = name.toStdString();
    dispatch(action);
}

void
BusConfig::changeMonoChannel(unsigned const ch)
{
    changeChannel(audio::bus_channel::mono, ch);
}

void
BusConfig::changeStereoLeftChannel(unsigned const ch)
{
    changeChannel(audio::bus_channel::left, ch);
}

void
BusConfig::changeStereoRightChannel(unsigned const ch)
{
    changeChannel(audio::bus_channel::right, ch);
}

void
BusConfig::changeChannel(audio::bus_channel const bc, unsigned const ch)
{
    runtime::actions::select_bus_channel action;
    action.bus_id = m_bus_id;
    action.channel_selector = bc;
    action.channel_index = static_cast<std::size_t>(ch) - 1;
    dispatch(action);
}

void
BusConfig::deleteBus()
{
    runtime::actions::delete_bus action;
    action.bus_id = m_bus_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
