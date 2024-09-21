// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam::runtime::ui
{

struct action
{
    virtual ~action() = default;

    [[nodiscard]] virtual auto clone() const -> std::unique_ptr<action> = 0;
};

template <class State>
struct reducible_action : action
{
    using state_t = State;

    virtual auto reduce(State&) const -> void = 0;
};

} // namespace piejam::runtime::ui
