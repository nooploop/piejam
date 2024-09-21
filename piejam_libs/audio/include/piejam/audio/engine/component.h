// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <span>

namespace piejam::audio::engine
{

class component
{
public:
    using endpoints = std::span<graph_endpoint const>;

    virtual ~component() = default;

    [[nodiscard]] virtual auto inputs() const -> endpoints = 0;
    [[nodiscard]] virtual auto outputs() const -> endpoints = 0;

    [[nodiscard]] constexpr auto num_inputs() const noexcept -> std::size_t
    {
        return inputs().size();
    }

    [[nodiscard]] constexpr auto num_outputs() const noexcept -> std::size_t
    {
        return outputs().size();
    }

    [[nodiscard]] virtual auto event_inputs() const -> endpoints = 0;
    [[nodiscard]] virtual auto event_outputs() const -> endpoints = 0;

    virtual void connect(graph&) const = 0;
};

} // namespace piejam::audio::engine
