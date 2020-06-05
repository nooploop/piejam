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

#include <vector>

namespace piejam::runtime::actions
{

struct toggle_input_channel final
    : ui::action<audio_state>
    , visitable_engine_action<toggle_input_channel>
{
    std::size_t index{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct set_input_channel_gain final
    : ui::action<audio_state>
    , visitable_engine_action<set_input_channel_gain>
{
    std::size_t index{};
    float gain{1.f};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct set_output_channel_gain final
    : ui::action<audio_state>
    , visitable_engine_action<set_output_channel_gain>
{
    float gain{1.f};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct request_levels_update final
    : ui::action<audio_state>
    , visitable_engine_action<request_levels_update>
{
    auto operator()(audio_state const&) const -> audio_state override;
};

struct update_levels final
    : ui::action<audio_state>
    , visitable_engine_action<update_levels>
{
    std::vector<float> in_levels;
    float out_level{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct request_info_update final
    : ui::action<audio_state>
    , visitable_engine_action<request_info_update>
{
    auto operator()(audio_state const&) const -> audio_state override;
};

struct update_info final
    : ui::action<audio_state>
    , visitable_engine_action<update_info>
{
    std::size_t xruns{};
    float cpu_load{};

    auto operator()(audio_state const&) const -> audio_state override;
};

} // namespace piejam::runtime::actions
