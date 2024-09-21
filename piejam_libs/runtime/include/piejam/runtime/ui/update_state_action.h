// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <concepts>
#include <functional>

namespace piejam::runtime::ui
{

template <class State>
struct update_state_action final
    : cloneable_action<update_state_action<State>, reducible_action<State>>
{
    using update_state_f = std::function<void(State&)>;

    update_state_action() = default;

    template <std::invocable<State&> UpdateState>
    update_state_action(UpdateState&& update_state)
        : m_update_state{std::forward<UpdateState>(update_state)}
    {
    }

    void reduce(State& st) const override
    {
        m_update_state(st);
    }

private:
    update_state_f m_update_state{[](State&) {}};
};

} // namespace piejam::runtime::ui
