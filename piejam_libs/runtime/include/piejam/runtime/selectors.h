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
#include <piejam/audio/period_sizes.h>
#include <piejam/audio/samplerates.h>
#include <piejam/audio/types.h>
#include <piejam/container/box.h>
#include <piejam/container/boxed_string.h>
#include <piejam/reselect/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/stereo_level.h>

#include <cstddef>

namespace piejam::runtime::selectors
{

template <class Value>
using selector = reselect::selector<Value, state>;

using samplerate = std::pair<container::box<audio::samplerates_t>, unsigned>;
extern const selector<samplerate> select_samplerate;

using period_size = std::pair<container::box<audio::period_sizes_t>, unsigned>;
extern const selector<period_size> select_period_size;

//! \todo input_devices/output_devices could get a reduced value, not the whole
//! pcm_io_descriptors
using input_devices =
        std::pair<container::box<audio::pcm_io_descriptors>, std::size_t>;
extern const selector<input_devices> select_input_devices;

using output_devices =
        std::pair<container::box<audio::pcm_io_descriptors>, std::size_t>;
extern const selector<output_devices> select_output_devices;

auto make_num_device_channels_selector(audio::bus_direction)
        -> selector<std::size_t>;

auto make_bus_list_selector(audio::bus_direction)
        -> selector<container::box<mixer::bus_list_t>>;

auto make_bus_name_selector(mixer::bus_id) -> selector<container::boxed_string>;

auto make_bus_type_selector(mixer::bus_id) -> selector<audio::bus_type>;

auto make_bus_channel_selector(mixer::bus_id, audio::bus_channel)
        -> selector<std::size_t>;

auto make_bus_volume_selector(mixer::bus_id) -> selector<float>;
auto make_bus_pan_balance_selector(mixer::bus_id) -> selector<float>;
auto make_bus_mute_selector(mixer::bus_id) -> selector<bool>;
auto make_bus_level_selector(mixer::bus_id) -> selector<stereo_level>;

auto make_input_solo_selector(mixer::bus_id) -> selector<bool>;

extern const selector<bool> select_input_solo_active;

extern const selector<mixer::bus_id> select_fx_chain_bus;

extern const selector<container::box<fx::chain_t>> select_current_fx_chain;
extern const selector<stereo_level> select_current_fx_chain_bus_level;
extern const selector<float> select_current_fx_chain_bus_volume;

auto make_fx_module_name_selector(fx::module_id)
        -> selector<container::boxed_string>;
auto make_fx_module_parameters_selector(fx::module_id)
        -> selector<container::box<fx::module_parameters>>;
auto make_fx_parameter_name_selector(fx::parameter_id)
        -> selector<container::boxed_string>;
auto make_fx_parameter_id_selector(fx::module_id, fx::parameter_key)
        -> selector<fx::parameter_id>;
auto make_fx_parameter_unit_selector(fx::parameter_id)
        -> selector<fx::parameter_unit>;

auto make_float_parameter_value_selector(float_parameter_id) -> selector<float>;
auto make_float_parameter_normalized_value_selector(float_parameter_id)
        -> selector<float>;

extern const selector<fx::registry> select_fx_registry;

extern const selector<std::size_t> select_xruns;
extern const selector<float> select_cpu_load;

} // namespace piejam::runtime::selectors
