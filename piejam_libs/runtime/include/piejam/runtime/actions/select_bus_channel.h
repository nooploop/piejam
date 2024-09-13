// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/external_audio_fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct select_bus_channel final
    : ui::cloneable_action<select_bus_channel, reducible_action>
    , visitable_engine_action<select_bus_channel>
{
    external_audio::bus_id bus_id{};
    audio::bus_channel channel_selector{};
    std::size_t channel_index{};

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
