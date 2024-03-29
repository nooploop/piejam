// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <piejam/audio/types.h>
#include <piejam/entity_id.h>

namespace piejam::runtime::actions
{

struct add_mixer_channel final
    : ui::cloneable_action<add_mixer_channel, reducible_action>
    , visitable_engine_action<add_mixer_channel>
{
    add_mixer_channel() = default;
    add_mixer_channel(std::string name, audio::bus_type bus_type)
        : name(std::move(name))
        , bus_type(bus_type)
    {
    }

    std::string name;
    audio::bus_type bus_type;
    bool auto_assign_input{};

    void reduce(state&) const override;
};

struct delete_mixer_channel final
    : ui::cloneable_action<delete_mixer_channel, reducible_action>
    , visitable_engine_action<delete_mixer_channel>
{
    mixer::channel_id mixer_channel_id{};

    void reduce(state&) const override;
};

auto initiate_mixer_channel_deletion(mixer::channel_id) -> thunk_action;

struct set_mixer_channel_name final
    : ui::cloneable_action<set_mixer_channel_name, reducible_action>
{
    mixer::channel_id channel_id;
    std::string name;

    void reduce(state&) const override;
};

template <io_direction D>
struct set_mixer_channel_route final
    : ui::cloneable_action<set_mixer_channel_route<D>, reducible_action>
    , visitable_engine_action<set_mixer_channel_route<D>>
{
    mixer::channel_id channel_id;
    mixer::io_address_t route;

    void reduce(state&) const override;
};

struct move_mixer_channel_left final
    : ui::cloneable_action<move_mixer_channel_left, reducible_action>
    , visitable_engine_action<move_mixer_channel_left>
{
    mixer::channel_id channel_id;

    void reduce(state&) const override;
};

struct move_mixer_channel_right final
    : ui::cloneable_action<move_mixer_channel_right, reducible_action>
    , visitable_engine_action<move_mixer_channel_right>
{
    mixer::channel_id channel_id;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
