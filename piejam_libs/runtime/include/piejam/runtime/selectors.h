// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/period_sizes.h>
#include <piejam/audio/samplerates.h>
#include <piejam/audio/types.h>
#include <piejam/box.h>
#include <piejam/boxed_string.h>
#include <piejam/boxed_vector.h>
#include <piejam/entity_id.h>
#include <piejam/io_direction.h>
#include <piejam/midi/device_id.h>
#include <piejam/reselect/fwd.h>
#include <piejam/runtime/device_io_fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/midi_assignment_id.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/stereo_level.h>

#include <cstddef>
#include <optional>

namespace piejam::runtime::selectors
{

template <class Value>
using selector = reselect::selector<Value, state>;

using samplerate = std::pair<box<audio::samplerates_t>, unsigned>;
extern const selector<samplerate> select_samplerate;

using period_size = std::pair<box<audio::period_sizes_t>, unsigned>;
extern const selector<period_size> select_period_size;

//! \todo input_devices/output_devices could get a reduced value, not the whole
//! pcm_io_descriptors
using input_devices = std::pair<box<audio::pcm_io_descriptors>, std::size_t>;
extern const selector<input_devices> select_input_devices;

using output_devices = std::pair<box<audio::pcm_io_descriptors>, std::size_t>;
extern const selector<output_devices> select_output_devices;

auto make_num_device_channels_selector(io_direction) -> selector<std::size_t>;

auto make_device_bus_list_selector(io_direction)
        -> selector<box<device_io::bus_list_t>>;

struct mixer_bus_info
{
    mixer::bus_id bus_id;
    float_parameter_id volume;
    float_parameter_id pan_balance;
    bool_parameter_id mute;
    stereo_level_parameter_id level;

    constexpr bool operator==(mixer_bus_info const&) const noexcept = default;
};

extern const selector<boxed_vector<mixer_bus_info>> select_mixer_bus_infos;
extern const selector<box<mixer_bus_info>> select_mixer_main_bus_info;

struct mixer_device_route
{
    device_io::bus_id bus_id;
    std::string name;

    bool operator==(mixer_device_route const&) const noexcept = default;
};

extern const selector<boxed_vector<mixer_device_route>>
        select_mixer_input_devices;
extern const selector<boxed_vector<mixer_device_route>>
        select_mixer_output_devices;

struct mixer_channel_route
{
    mixer::bus_id bus_id;
    std::string name;

    bool operator==(mixer_channel_route const&) const noexcept = default;
};

auto make_default_mixer_channel_input_is_valid_selector(mixer::bus_id)
        -> selector<bool>;

auto make_mixer_input_channels_selector(mixer::bus_id)
        -> selector<boxed_vector<mixer_channel_route>>;

auto make_mixer_output_channels_selector(mixer::bus_id)
        -> selector<boxed_vector<mixer_channel_route>>;

struct selected_route
{
    bool valid{};
    std::string name;

    bool operator==(selected_route const&) const noexcept = default;
};

auto make_mixer_channel_input_selector(mixer::bus_id)
        -> selector<box<selected_route>>;
auto make_mixer_channel_output_selector(mixer::bus_id)
        -> selector<box<selected_route>>;

auto make_device_bus_name_selector(device_io::bus_id) -> selector<boxed_string>;

auto make_mixer_bus_name_selector(mixer::bus_id) -> selector<boxed_string>;

auto make_bus_type_selector(device_io::bus_id) -> selector<audio::bus_type>;

auto make_bus_channel_selector(device_io::bus_id, audio::bus_channel)
        -> selector<std::size_t>;

extern const selector<boxed_vector<midi::device_id_t>>
        select_midi_input_devices;

auto make_midi_device_name_selector(midi::device_id_t)
        -> selector<boxed_string>;

auto make_midi_device_enabled_selector(midi::device_id_t) -> selector<bool>;

auto make_solo_selector(io_direction, mixer::bus_id) -> selector<bool>;

extern const selector<bool> select_input_solo_active;
extern const selector<bool> select_output_solo_active;

extern const selector<mixer::bus_id> select_fx_chain_bus;

extern const selector<box<fx::chain_t>> select_current_fx_chain;
extern const selector<stereo_level> select_current_fx_chain_bus_level;
extern const selector<float> select_current_fx_chain_bus_volume;

auto make_fx_module_name_selector(fx::module_id) -> selector<boxed_string>;
auto make_fx_module_parameters_selector(fx::module_id)
        -> selector<box<fx::module_parameters>>;
auto make_fx_module_can_move_left_selector(fx::module_id) -> selector<bool>;
auto make_fx_module_can_move_right_selector(fx::module_id) -> selector<bool>;
auto make_fx_parameter_name_selector(fx::parameter_id)
        -> selector<boxed_string>;
auto make_fx_parameter_id_selector(fx::module_id, fx::parameter_key)
        -> selector<fx::parameter_id>;
auto make_fx_parameter_value_string_selector(fx::parameter_id)
        -> selector<std::string>;

auto make_bool_parameter_value_selector(bool_parameter_id) -> selector<bool>;
auto make_float_parameter_value_selector(float_parameter_id) -> selector<float>;
auto make_float_parameter_normalized_value_selector(float_parameter_id)
        -> selector<float>;
auto make_int_parameter_value_selector(int_parameter_id) -> selector<int>;
auto make_int_parameter_min_selector(int_parameter_id) -> selector<int>;
auto make_int_parameter_max_selector(int_parameter_id) -> selector<int>;

auto make_level_parameter_value_selector(stereo_level_parameter_id)
        -> selector<stereo_level>;

auto make_midi_assignment_selector(midi_assignment_id)
        -> selector<std::optional<midi_assignment>>;

extern const selector<bool> select_midi_learning;

extern const selector<fx::registry> select_fx_registry;

extern const selector<std::size_t> select_xruns;
extern const selector<float> select_cpu_load;

} // namespace piejam::runtime::selectors
