// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/ui/batch_action.h>

namespace piejam::runtime::ui
{

template <class State>
auto
batch_action<State>::reduce(State const&) const -> State
{
    throw std::runtime_error("batch_action is not reducible.");
}

} // namespace piejam::runtime::ui
