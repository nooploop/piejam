// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/types.h>

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

    virtual auto make_processor(samplerate_t) const
            -> std::unique_ptr<engine::processor> = 0;
};

auto load(plugin_descriptor const&) -> std::unique_ptr<plugin>;

} // namespace piejam::audio::ladspa
