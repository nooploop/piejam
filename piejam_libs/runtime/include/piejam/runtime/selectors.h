// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
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

using sample_rate = std::pair<box<audio::sample_rates_t>, audio::sample_rate>;
extern selector<sample_rate> const select_sample_rate;

using period_size = std::pair<box<audio::period_sizes_t>, audio::period_size>;
extern selector<period_size> const select_period_size;

using period_count =
        std::pair<box<audio::period_counts_t>, audio::period_count>;
extern selector<period_count> const select_period_count;

extern selector<float> const select_buffer_latency;

//! \todo input_devices/output_devices could get a reduced value, not the whole
//! pcm_io_descriptors
using input_devices = std::pair<box<audio::pcm_io_descriptors>, std::size_t>;
extern selector<input_devices> const select_input_devices;

using output_devices = std::pair<box<audio::pcm_io_descriptors>, std::size_t>;
extern selector<output_devices> const select_output_devices;

auto make_num_device_channels_selector(io_direction) -> selector<std::size_t>;

auto make_device_bus_list_selector(io_direction)
        -> selector<box<device_io::bus_list_t>>;

extern selector<boxed_vector<mixer::channel_id>> const
        select_mixer_input_channels;
extern selector<mixer::channel_id> const select_mixer_main_channel;

auto make_mixer_channel_bus_type_selector(mixer::channel_id)
        -> selector<audio::bus_type>;
auto make_mixer_channel_volume_parameter_selector(mixer::channel_id)
        -> selector<float_parameter_id>;
auto make_mixer_channel_pan_balance_parameter_selector(mixer::channel_id)
        -> selector<float_parameter_id>;
auto make_mixer_channel_record_parameter_selector(mixer::channel_id)
        -> selector<bool_parameter_id>;
auto make_mixer_channel_mute_parameter_selector(mixer::channel_id)
        -> selector<bool_parameter_id>;
auto make_mixer_channel_solo_parameter_selector(mixer::channel_id)
        -> selector<bool_parameter_id>;
auto make_mixer_channel_peak_level_parameter_selector(mixer::channel_id)
        -> selector<stereo_level_parameter_id>;
auto make_mixer_channel_rms_level_parameter_selector(mixer::channel_id)
        -> selector<stereo_level_parameter_id>;

struct mixer_device_route
{
    device_io::bus_id bus_id;
    std::string name;

    auto operator==(mixer_device_route const&) const noexcept -> bool = default;
};

extern selector<boxed_vector<mixer_device_route>> const
        select_mixer_mono_input_devices;
extern selector<boxed_vector<mixer_device_route>> const
        select_mixer_stereo_input_devices;
extern selector<boxed_vector<mixer_device_route>> const
        select_mixer_output_devices;

struct mixer_channel_route
{
    mixer::channel_id channel_id;
    std::string name;

    auto operator==(mixer_channel_route const&) const noexcept
            -> bool = default;
};

auto make_default_mixer_channel_input_is_valid_selector(mixer::channel_id)
        -> selector<bool>;

auto make_mixer_input_channels_selector(mixer::channel_id)
        -> selector<boxed_vector<mixer_channel_route>>;

auto make_mixer_output_channels_selector(mixer::channel_id)
        -> selector<boxed_vector<mixer_channel_route>>;

struct selected_route
{
    enum class state_t
    {
        invalid,
        valid,
        not_mixed
    };

    state_t state{};
    std::string name;

    auto operator==(selected_route const&) const noexcept -> bool = default;
};

auto make_mixer_channel_input_selector(mixer::channel_id)
        -> selector<box<selected_route>>;
auto make_mixer_channel_output_selector(mixer::channel_id)
        -> selector<box<selected_route>>;

auto make_device_bus_name_selector(device_io::bus_id) -> selector<boxed_string>;

auto make_mixer_channel_name_selector(mixer::channel_id)
        -> selector<boxed_string>;

auto make_mixer_channel_can_move_left_selector(mixer::channel_id)
        -> selector<bool>;
auto make_mixer_channel_can_move_right_selector(mixer::channel_id)
        -> selector<bool>;

auto make_bus_type_selector(device_io::bus_id) -> selector<audio::bus_type>;

auto make_bus_channel_selector(device_io::bus_id, audio::bus_channel)
        -> selector<std::size_t>;

extern selector<boxed_vector<midi::device_id_t>> const
        select_midi_input_devices;

auto make_midi_device_name_selector(midi::device_id_t)
        -> selector<boxed_string>;

auto make_midi_device_enabled_selector(midi::device_id_t) -> selector<bool>;

auto make_muted_by_solo_selector(mixer::channel_id) -> selector<bool>;

auto make_fx_chain_selector(mixer::channel_id) -> selector<box<fx::chain_t>>;

auto make_fx_module_instance_id_selector(fx::module_id)
        -> selector<fx::instance_id>;
auto make_fx_module_name_selector(fx::module_id) -> selector<boxed_string>;
auto make_fx_module_bus_type_selector(fx::module_id)
        -> selector<audio::bus_type>;
auto make_fx_module_bypass_selector(fx::module_id) -> selector<bool>;
auto make_fx_module_parameters_selector(fx::module_id)
        -> selector<box<fx::module_parameters>>;
auto make_fx_module_can_move_up_selector(mixer::channel_id) -> selector<bool>;
auto make_fx_module_can_move_down_selector(mixer::channel_id) -> selector<bool>;
auto make_fx_parameter_name_selector(fx::parameter_id)
        -> selector<boxed_string>;
auto make_fx_parameter_value_string_selector(fx::parameter_id)
        -> selector<std::string>;
auto make_fx_module_streams_selector(fx::module_id)
        -> selector<box<fx::module_streams>>;
auto make_audio_stream_selector(audio_stream_id)
        -> selector<audio_stream_buffer>;

auto make_bool_parameter_value_selector(bool_parameter_id) -> selector<bool>;
auto make_float_parameter_value_selector(float_parameter_id) -> selector<float>;
auto make_float_parameter_normalized_value_selector(float_parameter_id)
        -> selector<float>;
auto make_int_parameter_value_selector(int_parameter_id) -> selector<int>;
auto make_int_parameter_min_selector(int_parameter_id) -> selector<int>;
auto make_int_parameter_max_selector(int_parameter_id) -> selector<int>;
auto make_int_parameter_enum_values_selector(int_parameter_id)
        -> selector<std::vector<std::pair<std::string, int>>>;

auto make_level_parameter_value_selector(stereo_level_parameter_id)
        -> selector<stereo_level>;

auto make_midi_assignment_selector(midi_assignment_id)
        -> selector<std::optional<midi_assignment>>;

extern selector<bool> const select_midi_learning;

extern selector<fx::registry> const select_fx_registry;

extern selector<bool> const select_recording;

extern selector<std::size_t> const select_xruns;
extern selector<float> const select_cpu_load;

extern selector<root_view_mode> const select_root_view_mode;
extern selector<mixer::channel_id> const select_fx_browser_fx_chain;

extern selector<mixer::channel_id> const select_focused_fx_chain;
extern selector<fx::module_id> const select_focused_fx_module;
extern selector<boxed_string> const select_focused_fx_module_name;
extern selector<bool> const select_focused_fx_module_bypassed;

} // namespace piejam::runtime::selectors
