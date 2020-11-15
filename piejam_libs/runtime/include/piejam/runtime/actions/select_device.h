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

#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/actions/reducible_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

template <audio::bus_direction D>
struct select_device final
    : reducible_action<
              select_device<D>,
              ui::cloneable_action<select_device<D>, action>>
    , visitable_device_action<select_device<D>>
{
    selected_device device;
    unsigned samplerate{};
    unsigned period_size{};
};

template <audio::bus_direction D>
inline bool
operator==(select_device<D> const& l, select_device<D> const& r) noexcept
{
    return l.device == r.device;
}

} // namespace piejam::runtime::actions
