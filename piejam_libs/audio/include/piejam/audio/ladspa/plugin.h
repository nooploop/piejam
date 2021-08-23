// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>

#include <memory>
#include <span>

namespace piejam::audio::ladspa
{

struct plugin_descriptor;
struct port_descriptor;

class plugin
{
public:
    virtual ~plugin() = default;

    virtual auto descriptor() const -> plugin_descriptor const& = 0;

    virtual auto control_inputs() const -> std::span<port_descriptor const> = 0;

    virtual auto make_processor(sample_rate const&) const
            -> std::unique_ptr<engine::processor> = 0;
};

auto load(plugin_descriptor const&) -> std::unique_ptr<plugin>;

} // namespace piejam::audio::ladspa
