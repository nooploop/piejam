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

#include <span>

namespace piejam::audio::engine
{

class component
{
public:
    using endpoints = std::span<graph_endpoint const>;

    virtual ~component() = default;

    virtual auto inputs() const -> endpoints = 0;
    virtual auto outputs() const -> endpoints = 0;

    virtual auto event_inputs() const -> endpoints = 0;
    virtual auto event_outputs() const -> endpoints = 0;

    virtual void connect(graph&) = 0;
};

} // namespace piejam::audio::engine