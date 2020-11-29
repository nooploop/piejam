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
#include <piejam/indexed_access.h>
#include <piejam/npos.h>
#include <piejam/runtime/fx/module.h>
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

    stereo_level_parameters levels{};

    fx::modules_t fx_modules{};

    mixer::state mixer_state{};

    mixer::bus_id fx_chain_bus{};

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
void add_mixer_bus(
        audio_state&,
        std::string name,
        audio::bus_type type,
        channel_index_pair const& chs = channel_index_pair{npos});

template <audio::bus_direction D>
void remove_mixer_bus(audio_state&, std::size_t index);

template <audio::bus_direction D>
void clear_mixer_buses(audio_state&);

void add_fx_module(audio_state&, mixer::bus_id, fx::type);
void remove_fx_module(audio_state& st, fx::module_id id);

} // namespace piejam::runtime
