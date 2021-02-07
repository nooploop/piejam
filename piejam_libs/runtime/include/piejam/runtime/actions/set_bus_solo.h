// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/runtime/actions/engine_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

template <io_direction D>
struct set_bus_solo final
    : ui::cloneable_action<set_bus_solo<D>, action>
    , visitable_engine_action<set_bus_solo<D>>
{
    std::size_t index{};
    bool solo{};

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
