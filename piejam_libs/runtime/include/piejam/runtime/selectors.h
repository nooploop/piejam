// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/types.h>
#include <piejam/box.h>
#include <piejam/boxed_string.h>
#include <piejam/boxed_vector.h>
#include <piejam/entity_id.h>
#include <piejam/io_direction.h>
#include <piejam/midi/device_id.h>
#include <piejam/reselect/fwd.h>
#include <piejam/runtime/external_audio_fwd.h>
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

template <class Available, class Current>
struct choice_model
{
    Available available;
    Current current;

    auto operator==(choice_model const&) const -> bool = default;
};

using sample_rate_choice =
        choice_model<audio::sample_rates_t, audio::sample_rate>;
extern selector<box<sample_rate_choice>> const select_sample_rate;

using period_size_choice =
        choice_model<audio::period_sizes_t, audio::period_size>;
extern selector<box<period_size_choice>> const select_period_size;

using period_count_choice =
        choice_model<audio::period_counts_t, audio::period_count>;
extern selector<box<period_count_choice>> const select_period_count;

extern selector<float> const select_buffer_latency;

using sound_card_choice = choice_model<std::vector<std::string>, std::size_t>;
extern selector<box<sound_card_choice>> const select_input_sound_card;
extern selector<box<sound_card_choice>> const select_output_sound_card;

auto make_num_device_channels_selector(io_direction) -> selector<std::size_t>;

auto make_external_audio_device_ids_selector(io_direction)
        -> selector<unique_box<external_audio::device_ids_t>>;

extern selector<unique_box<mixer::channel_ids_t>> const
        select_mixer_user_channels;
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
    external_audio::device_id device_id;
    std::string name;

    auto operator==(mixer_device_route const&) const noexcept -> bool = default;
};

struct mixer_channel_route
{
    mixer::channel_id channel_id;
    std::string name;

    auto operator==(mixer_channel_route const&) const noexcept
            -> bool = default;
};

struct selected_route
{
    enum class state_t
    {
        invalid,
        valid,
        not_mixed
    };

    state_t state{};
    boxed_string name;

    auto operator==(selected_route const&) const noexcept -> bool = default;
};

auto make_mixer_channel_default_route_name_selector(
        audio::bus_type,
        mixer::io_socket) -> selector<boxed_string>;

auto make_mixer_channel_default_route_is_valid_selector(
        mixer::channel_id,
        mixer::io_socket) -> selector<bool>;

auto make_mixer_channel_selected_route_selector(
        mixer::channel_id,
        mixer::io_socket) -> selector<box<selected_route>>;

auto make_mixer_device_routes_selector(audio::bus_type, mixer::io_socket)
        -> selector<boxed_vector<mixer_device_route>>;

auto make_mixer_channel_routes_selector(mixer::channel_id, mixer::io_socket)
        -> selector<boxed_vector<mixer_channel_route>>;

auto make_external_audio_device_name_selector(external_audio::device_id)
        -> selector<boxed_string>;

auto make_mixer_channel_name_selector(mixer::channel_id)
        -> selector<boxed_string>;

auto make_mixer_channel_can_move_left_selector(mixer::channel_id)
        -> selector<bool>;
auto make_mixer_channel_can_move_right_selector(mixer::channel_id)
        -> selector<bool>;

auto make_external_audio_device_bus_type_selector(external_audio::device_id)
        -> selector<audio::bus_type>;

auto make_external_audio_device_bus_channel_selector(
        external_audio::device_id,
        audio::bus_channel) -> selector<std::size_t>;

extern selector<unique_box<midi::device_ids_t>> const select_midi_input_devices;

auto make_midi_device_name_selector(midi::device_id_t)
        -> selector<boxed_string>;

auto make_midi_device_enabled_selector(midi::device_id_t) -> selector<bool>;

auto make_muted_by_solo_selector(mixer::channel_id) -> selector<bool>;

auto make_fx_chain_selector(mixer::channel_id)
        -> selector<unique_box<fx::chain_t>>;

auto make_fx_module_instance_id_selector(fx::module_id)
        -> selector<fx::instance_id>;
auto make_fx_module_name_selector(fx::module_id) -> selector<boxed_string>;
auto make_fx_module_bus_type_selector(fx::module_id)
        -> selector<audio::bus_type>;
auto make_fx_module_bypass_selector(fx::module_id) -> selector<bool>;
auto make_fx_module_parameters_selector(fx::module_id)
        -> selector<unique_box<fx::module_parameters>>;
auto make_fx_module_can_move_up_selector(mixer::channel_id) -> selector<bool>;
auto make_fx_module_can_move_down_selector(mixer::channel_id) -> selector<bool>;
auto make_fx_parameter_name_selector(fx::parameter_id)
        -> selector<boxed_string>;
auto make_fx_parameter_value_string_selector(fx::parameter_id)
        -> selector<std::string>;
auto make_fx_module_streams_selector(fx::module_id)
        -> selector<unique_box<fx::module_streams>>;
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
