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

    bool operator<(graph_endpoint const& other) const noexcept
    {
        auto const proc_l = std::addressof(proc.get());
        auto const proc_r = std::addressof(other.proc.get());
        return proc_l < proc_r || (proc_l == proc_r && port < other.port);
    }

    bool operator==(graph_endpoint const& other) const noexcept
    {
        auto const proc_l = std::addressof(proc.get());
        auto const proc_r = std::addressof(other.proc.get());
        return proc_l == proc_r && port == other.port;
    }
};

} // namespace piejam::audio::engine
