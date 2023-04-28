// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <functional>

namespace piejam::audio::engine
{

struct graph_endpoint
{
    std::reference_wrapper<processor> proc;
    std::size_t port{};

    auto operator<(graph_endpoint const& other) const noexcept -> bool
    {
        auto* const proc_l = std::addressof(proc.get());
        auto* const proc_r = std::addressof(other.proc.get());
        return proc_l < proc_r || (proc_l == proc_r && port < other.port);
    }

    auto operator==(graph_endpoint const& other) const noexcept -> bool
    {
        auto* const proc_l = std::addressof(proc.get());
        auto* const proc_r = std::addressof(other.proc.get());
        return proc_l == proc_r && port == other.port;
    }
};

template <std::size_t Port>
inline constexpr auto make_graph_endpoint = [](processor& proc) {
    return graph_endpoint{.proc = proc, .port = Port};
};

} // namespace piejam::audio::engine
