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

#include <piejam/audio/bus_defs.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/container/box.h>
#include <piejam/runtime/actions/device_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/selected_device.h>

namespace piejam::runtime::actions
{

struct refresh_devices final
    : action
    , visitable_device_action<refresh_devices>
{
    auto operator()(audio_state const&) const -> audio_state override;
};

struct update_devices final
    : action
    , visitable_device_action<update_devices>
{
    container::box<piejam::audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    unsigned samplerate{};
    unsigned period_size{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct initiate_device_selection final
    : action
    , visitable_device_action<initiate_device_selection>
{
    bool input{};
    std::size_t index{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct select_device final
    : action
    , visitable_device_action<select_device>
{
    bool input{};
    selected_device device;
    unsigned samplerate{};
    unsigned period_size{};

    auto operator()(audio_state const&) const -> audio_state override;
};

inline bool
operator==(select_device const& l, select_device const& r) noexcept
{
    return l.input == r.input && l.device == r.device;
}

struct select_samplerate final
    : action
    , visitable_device_action<select_samplerate>
{
    std::size_t index{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct select_period_size final
    : action
    , visitable_device_action<select_period_size>
{
    std::size_t index{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct select_bus_channel final
    : action
    , visitable_device_action<select_bus_channel>
{
    audio::bus_direction direction{};
    std::size_t bus{};
    audio::bus_channel channel_selector{};
    std::size_t channel_index{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct add_device_bus final
    : action
    , visitable_device_action<add_device_bus>
{
    audio::bus_direction direction{};
    audio::bus_type type{};

    auto operator()(audio_state const&) const -> audio_state override;
};

struct delete_device_bus final
    : action
    , visitable_device_action<delete_device_bus>
{
    std::size_t bus{};

    auto operator()(audio_state const&) const -> audio_state override;
};

} // namespace piejam::runtime::actions
