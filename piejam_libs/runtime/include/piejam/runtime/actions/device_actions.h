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

#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/types.h>
#include <piejam/container/box.h>
#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/selected_device.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct refresh_devices final
    : ui::cloneable_action<refresh_devices, action>
    , visitable_device_action<refresh_devices>
{
    auto reduce(audio_state const&) const -> audio_state override;
};

struct update_devices final
    : ui::cloneable_action<update_devices, action>
    , visitable_device_action<update_devices>
{
    container::box<piejam::audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    unsigned samplerate{};
    unsigned period_size{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct initiate_device_selection final
    : ui::cloneable_action<initiate_device_selection, action>
    , visitable_device_action<initiate_device_selection>
{
    bool input{};
    std::size_t index{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct select_device final
    : ui::cloneable_action<select_device, action>
    , visitable_device_action<select_device>
{
    bool input{};
    selected_device device;
    unsigned samplerate{};
    unsigned period_size{};

    auto reduce(audio_state const&) const -> audio_state override;
};

inline bool
operator==(select_device const& l, select_device const& r) noexcept
{
    return l.input == r.input && l.device == r.device;
}

struct select_samplerate final
    : ui::cloneable_action<select_samplerate, action>
    , visitable_device_action<select_samplerate>
{
    std::size_t index{};

    auto reduce(audio_state const&) const -> audio_state override;
};

struct select_period_size final
    : ui::cloneable_action<select_period_size, action>
    , visitable_device_action<select_period_size>
{
    std::size_t index{};

    auto reduce(audio_state const&) const -> audio_state override;
};

} // namespace piejam::runtime::actions
