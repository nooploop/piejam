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

#include <piejam/audio/fwd.h>
#include <piejam/container/box.h>
#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/selected_device.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct update_devices final
    : ui::cloneable_action<update_devices, action>
    , visitable_device_action<update_devices>
{
    container::box<piejam::audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    audio::samplerate_t samplerate{};
    audio::period_size_t period_size{};

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
