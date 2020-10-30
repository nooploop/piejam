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

#include <functional>
#include <span>
#include <string_view>

namespace piejam::audio::engine
{

class processor
{
public:
    using event_ports = std::span<event_port const>;

    virtual ~processor() = default;

    virtual auto type_name() const -> std::string_view = 0;
    virtual auto name() const -> std::string_view = 0;

    virtual auto num_inputs() const -> std::size_t = 0;
    virtual auto num_outputs() const -> std::size_t = 0;

    virtual auto event_inputs() const -> event_ports = 0;
    virtual auto event_outputs() const -> event_ports = 0;

    virtual void
    create_event_output_buffers(event_output_buffer_factory const&) const = 0;

    virtual void process(process_context const&) = 0;
};

} // namespace piejam::audio::engine
