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
#include <piejam/npos.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/selected_device.h>

#include <functional>
#include <vector>

namespace piejam::runtime
{

struct audio_state
{
    container::box<audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    audio::samplerate_t samplerate{};
    audio::period_size_t period_size{};

    float_parameters float_params{};
    bool_parameters bool_params{};

    mixer::state mixer_state{};

    std::size_t xruns{};
    float cpu_load{};
};

auto samplerates(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params)
        -> audio::samplerates_t;
auto samplerates_from_state(audio_state const&) -> audio::samplerates_t;

auto period_sizes(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params)
        -> audio::period_sizes_t;
auto period_sizes_from_state(audio_state const&) -> audio::period_sizes_t;

template <audio::bus_direction D>
auto
add_mixer_bus(
        audio_state& st,
        std::string name,
        audio::bus_type type,
        channel_index_pair chs = channel_index_pair{npos})
{
    auto volume_id = st.float_params.add(
            parameter::float_{.default_value = 1.f, .min = 0.f, .max = 8.f});
    auto pan_balance_id = st.float_params.add(
            parameter::float_{.default_value = 0.f, .min = -1.f, .max = 1.f});
    auto mute_id = st.bool_params.add(parameter::bool_{.default_value = false});

    mixer::bus_list_t bus_ids = mixer::bus_ids<D>(st.mixer_state);
    bus_ids.emplace_back(st.mixer_state.buses.add(mixer::bus{
            .name = std::move(name),
            .volume = volume_id,
            .pan_balance = pan_balance_id,
            .mute = mute_id,
            .type = type,
            .device_channels = std::move(chs)}));
    mixer::bus_ids<D>(st.mixer_state) = bus_ids;
}

template <audio::bus_direction D>
auto
remove_mixer_bus(audio_state& st, std::size_t index)
{
    mixer::bus_list_t bus_ids = mixer::bus_ids<D>(st.mixer_state);
    BOOST_ASSERT(index < bus_ids.size());
    mixer::bus_id const bus_id = bus_ids[index];

    if constexpr (D == audio::bus_direction::input)
    {
        if (st.mixer_state.input_solo_id == bus_id)
            st.mixer_state.input_solo_id = mixer::bus_id{};
    }

    mixer::bus const& bus = st.mixer_state.buses[bus_id];
    st.float_params.remove(bus.volume);
    st.float_params.remove(bus.pan_balance);
    st.bool_params.remove(bus.mute);
    st.mixer_state.buses.remove(bus_id);
    bus_ids.erase(bus_ids.begin() + index);

    mixer::bus_ids<D>(st.mixer_state) = bus_ids;
}

template <audio::bus_direction D>
auto
clear_mixer_buses(audio_state& st)
{
    std::size_t num_buses = mixer::bus_ids<D>(st.mixer_state)->size();
    while (num_buses--)
        remove_mixer_bus<D>(st, num_buses);
}

} // namespace piejam::runtime
