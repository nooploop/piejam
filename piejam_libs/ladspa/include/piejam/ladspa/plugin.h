// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>

#include <memory>
#include <span>

namespace piejam::ladspa
{

struct plugin_descriptor;
struct port_descriptor;

class plugin
{
public:
    virtual ~plugin() = default;

    [[nodiscard]] virtual auto descriptor() const
            -> plugin_descriptor const& = 0;

    [[nodiscard]] virtual auto control_inputs() const
            -> std::span<port_descriptor const> = 0;

    [[nodiscard]] virtual auto make_processor(audio::sample_rate) const
            -> std::unique_ptr<audio::engine::processor> = 0;
};

auto load(plugin_descriptor const&) -> std::unique_ptr<plugin>;

} // namespace piejam::ladspa
