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

#pragma once

#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <vector>

namespace piejam::runtime::actions
{

struct select_bus_channel final
    : ui::cloneable_action<select_bus_channel, action>
    , visitable_engine_action<select_bus_channel>
{
    audio::bus_direction direction{};
    std::size_t bus{};
    audio::bus_channel channel_selector{};
    std::size_t channel_index{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct add_device_bus final
    : ui::cloneable_action<add_device_bus, action>
    , visitable_engine_action<add_device_bus>
{
    audio::bus_direction direction{};
    audio::bus_type type{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct delete_device_bus final
    : ui::cloneable_action<delete_device_bus, action>
    , visitable_engine_action<delete_device_bus>
{
    audio::bus_direction direction{};
    std::size_t bus{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_input_channel_volume final
    : ui::cloneable_action<set_input_channel_volume, action>
    , visitable_engine_action<set_input_channel_volume>
{
    std::size_t index{};
    float volume{1.f};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_input_channel_pan final
    : ui::cloneable_action<set_input_channel_pan, action>
    , visitable_engine_action<set_input_channel_pan>
{
    std::size_t index{};
    float pan{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_input_channel_mute final
    : ui::cloneable_action<set_input_channel_mute, action>
    , visitable_engine_action<set_input_channel_mute>
{
    std::size_t index{};
    bool mute{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_input_solo final
    : ui::cloneable_action<set_input_solo, action>
    , visitable_engine_action<set_input_solo>
{
    std::size_t index{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_output_channel_volume final
    : ui::cloneable_action<set_output_channel_volume, action>
    , visitable_engine_action<set_output_channel_volume>
{
    std::size_t index{};
    float volume{1.f};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_output_channel_balance final
    : ui::cloneable_action<set_output_channel_balance, action>
    , visitable_engine_action<set_output_channel_balance>
{
    std::size_t index{};
    float balance{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct set_output_channel_mute final
    : ui::cloneable_action<set_output_channel_mute, action>
    , visitable_engine_action<set_output_channel_mute>
{
    std::size_t index{};
    bool mute{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct request_levels_update final
    : ui::cloneable_action<request_levels_update, action>
    , visitable_engine_action<request_levels_update>
{
    auto reduce(audio_state const&) const -> audio_state override;
};

struct update_levels final
    : ui::cloneable_action<update_levels, action>
    , visitable_engine_action<update_levels>
{
    std::vector<mixer::stereo_level> in_levels;
    std::vector<mixer::stereo_level> out_levels;

    auto reduce(audio_state const&) const -> audio_state override;
};

struct request_info_update final
    : ui::cloneable_action<request_info_update, action>
    , visitable_engine_action<request_info_update>
{
    auto reduce(audio_state const&) const -> audio_state override;
};

struct update_info final
    : ui::cloneable_action<update_info, action>
    , visitable_engine_action<update_info>
{
    std::size_t xruns{};
    float cpu_load{};

    auto reduce(audio_state const&) const -> audio_state override;
};

} // namespace piejam::runtime::actions
