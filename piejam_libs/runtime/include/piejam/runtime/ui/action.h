// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam::runtime::ui
{

template <class State>
struct action
{
    using state_t = State;

    virtual ~action() = default;

    virtual auto clone() const -> std::unique_ptr<action> = 0;

    virtual auto reduce(State const&) const -> State = 0;

    bool operator==(action const&) const noexcept = default;
};

} // namespace piejam::runtime::ui
