// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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

    auto reduce(State const& st) const -> State override
    {
        auto new_st = st;
        m_update_state(new_st);
        return new_st;
    }

private:
    update_state_f m_update_state{[](State&) {}};
};

} // namespace piejam::runtime::ui
