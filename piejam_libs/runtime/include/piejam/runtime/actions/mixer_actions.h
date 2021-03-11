// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct add_mixer_channel final
    : ui::cloneable_action<add_mixer_channel, action>
    , visitable_engine_action<add_mixer_channel>
{
    add_mixer_channel() = default;
    add_mixer_channel(std::string name)
        : name(std::move(name))
    {
    }

    std::string name;

    auto reduce(state const&) const -> state override;
};

struct delete_mixer_channel final
    : ui::cloneable_action<delete_mixer_channel, action>
    , visitable_engine_action<delete_mixer_channel>
{
    mixer::channel_id channel_id{};

    auto reduce(state const&) const -> state override;
};

auto initiate_mixer_channel_deletion(mixer::channel_id) -> thunk_action;

struct set_mixer_channel_name final
    : ui::cloneable_action<set_mixer_channel_name, action>
{
    mixer::channel_id channel_id;
    std::string name;

    auto reduce(state const&) const -> state override;
};

template <io_direction D>
struct set_mixer_channel_route final
    : ui::cloneable_action<set_mixer_channel_route<D>, action>
    , visitable_engine_action<set_mixer_channel_route<D>>
{
    mixer::channel_id channel_id;
    mixer::io_address_t route;

    auto reduce(state const&) const -> state override;
};

struct move_mixer_channel_left final
    : ui::cloneable_action<move_mixer_channel_left, action>
    , visitable_engine_action<move_mixer_channel_left>
{
    mixer::channel_id channel_id;

    auto reduce(state const&) const -> state override;
};

struct move_mixer_channel_right final
    : ui::cloneable_action<move_mixer_channel_right, action>
    , visitable_engine_action<move_mixer_channel_right>
{
    mixer::channel_id channel_id;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
