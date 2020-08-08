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

#include <piejam/audio/types.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>

#include <string>

namespace piejam::runtime::actions
{

struct set_bus_name : action
{
    audio::bus_direction bus_direction{};
    std::size_t bus{};
    std::string name;

    auto operator()(audio_state const&) const -> audio_state override;
};

} // namespace piejam::runtime::actions
