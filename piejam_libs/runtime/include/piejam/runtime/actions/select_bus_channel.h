// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct select_bus_channel final
    : ui::cloneable_action<select_bus_channel, action>
    , visitable_engine_action<select_bus_channel>
{
    device_io::bus_id bus_id{};
    audio::bus_channel channel_selector{};
    std::size_t channel_index{};

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
