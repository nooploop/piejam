// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/selectors.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/npos.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/solo_group.h>
#include <piejam/runtime/state.h>

#include <boost/hof/match.hpp>
#include <boost/hof/unpack.hpp>

#include <cassert>

namespace piejam::runtime::selectors
{

const selector<sample_rate> select_sample_rate(
        [get_sample_rates = memo(boxify_result(&runtime::sample_rates))](
                state const& st) -> sample_rate {
            return {get_sample_rates(st.input.hw_params, st.output.hw_params),
                    st.sample_rate};
        });

const selector<period_size> select_period_size(
        [get_period_sizes = memo(boxify_result(&runtime::period_sizes))](
                state const& st) -> period_size {
            return {get_period_sizes(st.input.hw_params, st.output.hw_params),
                    st.period_size};
        });

const selector<period_count> select_period_count(
        [get = memo(boxify_result(&runtime::period_counts))](state const& st)
                -> period_count {
            return {get(st.input.hw_params, st.output.hw_params),
                    st.period_count};
        });

const selector<float> select_buffer_latency([](state const& st) {
    return st.sample_rate.get() != 0
                   ? (st.period_size.get() * st.period_count.get() * 1000.f) /
                             st.sample_rate.as_float()
                   : 0.f;
});

const selector<input_devices>
        select_input_devices([](state const& st) -> input_devices {
            return {st.pcm_devices, st.input.index};
        });

const selector<output_devices>
        select_output_devices([](state const& st) -> output_devices {
            return {st.pcm_devices, st.output.index};
        });

auto
make_num_device_channels_selector(io_direction const bd)
        -> selector<std::size_t>
{
    switch (bd)
    {
        case io_direction::input:
            return selector<std::size_t>([](state const& st) -> std::size_t {
                return st.input.hw_params->num_channels;
            });

        case io_direction::output:
            return selector<std::size_t>([](state const& st) -> std::size_t {
                return st.output.hw_params->num_channels;
            });
    }
}

auto
make_device_bus_list_selector(io_direction const bd)
        -> selector<box<device_io::bus_list_t>>
{
    switch (bd)
    {
        case io_direction::input:
            return [](state const& st) { return st.device_io_state.inputs; };

        case io_direction::output:
            return [](state const& st) { return st.device_io_state.outputs; };
    }
}

const selector<boxed_vector<mixer::channel_id>> select_mixer_input_channels(
        [](state const& st) { return st.mixer_state.inputs; });

const selector<mixer::channel_id> select_mixer_main_channel(
        [](state const& st) { return st.mixer_state.main; });

auto
make_mixer_channel_bus_type_selector(mixer::channel_id const channel_id)
        -> selector<audio::bus_type>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->bus_type : audio::bus_type::mono;
    };
}

auto
make_mixer_channel_volume_parameter_selector(mixer::channel_id const channel_id)
        -> selector<float_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->volume : float_parameter_id{};
    };
}

auto
make_mixer_channel_pan_balance_parameter_selector(
        mixer::channel_id const channel_id) -> selector<float_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->pan_balance : float_parameter_id{};
    };
}

auto
make_mixer_channel_record_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->record : bool_parameter_id{};
    };
}

auto
make_mixer_channel_mute_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->mute : bool_parameter_id{};
    };
}

auto
make_mixer_channel_solo_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->solo : bool_parameter_id{};
    };
}

auto
make_mixer_channel_level_parameter_selector(mixer::channel_id const channel_id)
        -> selector<stereo_level_parameter_id>
{
    return [channel_id](state const& st) {
        mixer::channel const* const channel =
                st.mixer_state.channels.find(channel_id);
        return channel ? channel->level : stereo_level_parameter_id{};
    };
}

static auto
make_mixer_device_routes(
        device_io::buses_t const& devices,
        box<device_io::bus_list_t> const& from_device_list,
        audio::bus_type bus_type)
{
    std::vector<mixer_device_route> result;
    for (auto device_bus_id : *from_device_list)
    {
        if (device_io::bus const* const device_bus =
                    devices.find(device_bus_id);
            device_bus && device_bus->bus_type == bus_type)
        {
            result.emplace_back(mixer_device_route{
                    .bus_id = device_bus_id,
                    .name = *device_bus->name});
        }
    }
    return result;
}

const selector<boxed_vector<mixer_device_route>>
        select_mixer_mono_input_devices(
                [get = memo(boxify_result(&make_mixer_device_routes))](
                        state const& st) mutable {
                    return get(
                            st.device_io_state.buses,
                            st.device_io_state.inputs,
                            audio::bus_type::mono);
                });

const selector<boxed_vector<mixer_device_route>>
        select_mixer_stereo_input_devices(
                [get = memo(boxify_result(&make_mixer_device_routes))](
                        state const& st) mutable {
                    return get(
                            st.device_io_state.buses,
                            st.device_io_state.inputs,
                            audio::bus_type::stereo);
                });

const selector<boxed_vector<mixer_device_route>> select_mixer_output_devices(
        [get = memo(boxify_result(&make_mixer_device_routes))](
                state const& st) mutable {
            return get(
                    st.device_io_state.buses,
                    st.device_io_state.outputs,
                    audio::bus_type::stereo);
        });

auto
make_default_mixer_channel_input_is_valid_selector(
        mixer::channel_id const bus_id) -> selector<bool>
{
    return [bus_id,
            get = memo(&mixer::is_default_source_valid)](state const& st) {
        return get(st.mixer_state.channels, bus_id);
    };
}

static auto
make_mixer_input_channel_routes(
        mixer::channels_t const& channels,
        mixer::channel_id const channel_id)
{
    auto valid_sources = mixer::valid_source_channels(channels, channel_id);
    return algorithm::transform_to_vector(valid_sources, [&](auto const& id) {
        return mixer_channel_route{.channel_id = id, .name = channels[id].name};
    });
}

auto
make_mixer_input_channels_selector(mixer::channel_id const channel_id)
        -> selector<boxed_vector<mixer_channel_route>>
{
    return [channel_id,
            get = memo(boxify_result(&make_mixer_input_channel_routes))](
                   state const& st) {
        return get(st.mixer_state.channels, channel_id);
    };
}

static auto
make_mixer_output_channel_routes(
        mixer::channels_t const& channels,
        mixer::channel_id const channel_id)
{
    auto valid_targets = mixer::valid_target_channels(channels, channel_id);
    return algorithm::transform_to_vector(valid_targets, [&](auto const& id) {
        return mixer_channel_route{.channel_id = id, .name = channels[id].name};
    });
}

auto
make_mixer_output_channels_selector(mixer::channel_id const channel_id)
        -> selector<boxed_vector<mixer_channel_route>>
{
    return [channel_id,
            get = memo(boxify_result(&make_mixer_output_channel_routes))](
                   state const& st) {
        return get(st.mixer_state.channels, channel_id);
    };
}

static auto
get_mixer_channel_selected_input(
        mixer::channel_id const channel_id,
        mixer::channels_t const& channels,
        device_io::buses_t const& device_buses) -> selected_route
{
    static std::string s_none{"None"};
    static std::string s_mix{"Mix"};

    mixer::channel const* const mixer_channel = channels.find(channel_id);
    if (!mixer_channel)
    {
        return selected_route{selected_route::state_t::invalid, s_mix};
    }

    return std::visit(
            boost::hof::match(
                    [mono = mixer_channel->bus_type ==
                            audio::bus_type::mono](default_t) {
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = mono ? s_none : s_mix};
                    },
                    [&](mixer::channel_id const src_bus_id) {
                        mixer::channel const* const src_bus =
                                channels.find(src_bus_id);
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = src_bus ? *src_bus->name : s_mix};
                    },
                    [&](device_io::bus_id const dev_bus_id) {
                        device_io::bus const* const dev_bus =
                                device_buses.find(dev_bus_id);
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = dev_bus ? *dev_bus->name : s_mix};
                    },
                    [](mixer::missing_device_address const& name) {
                        return selected_route{
                                .state = selected_route::state_t::invalid,
                                .name = name};
                    }),
            mixer_channel->in);
}

auto
make_mixer_channel_input_selector(mixer::channel_id const channel_id)
        -> selector<box<selected_route>>
{
    return [channel_id,
            get = memo(boxify_result(&get_mixer_channel_selected_input))](
                   state const& st) {
        return get(
                channel_id,
                st.mixer_state.channels,
                st.device_io_state.buses);
    };
}

static auto
get_mixer_channel_selected_output(
        mixer::channel_id const channel_id,
        mixer::channels_t const& channels,
        device_io::buses_t const& device_buses) -> selected_route
{
    static std::string s_none{"None"};

    mixer::channel const* const mixer_channel = channels.find(channel_id);
    if (!mixer_channel)
    {
        return selected_route{selected_route::state_t::invalid, s_none};
    }

    return std::visit(
            boost::hof::match(
                    [](default_t) {
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = s_none};
                    },
                    [&](mixer::channel_id const target_channel_id) {
                        if (mixer::channel const* const target_channel =
                                    channels.find(target_channel_id))
                        {
                            return selected_route{
                                    .state = std::holds_alternative<default_t>(
                                                     target_channel->in)
                                                     ? selected_route::state_t::
                                                               valid
                                                     : selected_route::state_t::
                                                               not_mixed,
                                    .name = target_channel->name};
                        }
                        else
                        {
                            return selected_route{
                                    .state = selected_route::state_t::valid,
                                    .name = s_none};
                        }
                    },
                    [&](device_io::bus_id const dev_bus_id) {
                        device_io::bus const* const dev_bus =
                                device_buses.find(dev_bus_id);
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = dev_bus ? *dev_bus->name : s_none};
                    },
                    [](mixer::missing_device_address const& name) {
                        return selected_route{
                                .state = selected_route::state_t::invalid,
                                .name = name};
                    }),
            mixer_channel->out);
}

auto
make_mixer_channel_output_selector(mixer::channel_id const channel_id)
        -> selector<box<selected_route>>
{
    return [channel_id,
            get = memo(boxify_result(&get_mixer_channel_selected_output))](
                   state const& st) {
        return get(
                channel_id,
                st.mixer_state.channels,
                st.device_io_state.buses);
    };
}

auto
make_device_bus_name_selector(device_io::bus_id const bus_id)
        -> selector<boxed_string>
{
    auto get_device_bus_name = [](device_io::buses_t const& device_buses,
                                  device_io::bus_id const bus_id) {
        device_io::bus const* const bus = device_buses.find(bus_id);
        return bus ? bus->name : boxed_string();
    };

    return [bus_id,
            get = memo(get_device_bus_name)](state const& st) -> boxed_string {
        return get(st.device_io_state.buses, bus_id);
    };
}

auto
make_mixer_channel_name_selector(mixer::channel_id const channel_id)
        -> selector<boxed_string>
{
    auto get_mixer_channel_name = [](mixer::channels_t const& channels,
                                     mixer::channel_id const channel_id) {
        mixer::channel const* const mixer_channel = channels.find(channel_id);
        return mixer_channel ? mixer_channel->name : boxed_string();
    };

    return [channel_id, get = memo(get_mixer_channel_name)](
                   state const& st) -> boxed_string {
        return get(st.mixer_state.channels, channel_id);
    };
}

auto
make_mixer_channel_can_move_left_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return [channel_id](state const& st) {
        return channel_id != st.mixer_state.main &&
               !st.mixer_state.inputs->empty() &&
               channel_id != st.mixer_state.inputs->front();
    };
}

auto
make_mixer_channel_can_move_right_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return [channel_id](state const& st) {
        return channel_id != st.mixer_state.main &&
               !st.mixer_state.inputs->empty() &&
               channel_id != st.mixer_state.inputs->back();
    };
}

auto
make_bus_type_selector(device_io::bus_id const bus_id)
        -> selector<audio::bus_type>
{
    return [bus_id](state const& st) -> audio::bus_type {
        device_io::bus const* bus = st.device_io_state.buses.find(bus_id);
        return bus ? bus->bus_type : audio::bus_type::mono;
    };
}

auto
make_bus_channel_selector(
        device_io::bus_id const bus_id,
        audio::bus_channel const bc) -> selector<std::size_t>
{
    switch (bc)
    {
        case audio::bus_channel::mono:
        case audio::bus_channel::left:
            return [bus_id](state const& st) -> std::size_t {
                device_io::bus const* const bus =
                        st.device_io_state.buses.find(bus_id);
                return bus ? bus->channels.left : piejam::npos;
            };

        case audio::bus_channel::right:
            return [bus_id](state const& st) -> std::size_t {
                device_io::bus const* const bus =
                        st.device_io_state.buses.find(bus_id);
                return bus ? bus->channels.right : piejam::npos;
            };
    }

    BOOST_ASSERT(false);
    __builtin_unreachable();
}

const selector<boxed_vector<midi::device_id_t>> select_midi_input_devices(
        [](state const& st) -> boxed_vector<midi::device_id_t> {
            return st.midi_inputs;
        });

static auto
midi_device_name(
        box<std::unordered_map<midi::device_id_t, midi_device_config>> const&
                midi_devices,
        midi::device_id_t const device_id) -> boxed_string
{
    auto it = midi_devices->find(device_id);
    return it != midi_devices->end() ? it->second.name : boxed_string();
}

auto
make_midi_device_name_selector(midi::device_id_t const device_id)
        -> selector<boxed_string>
{
    return [device_id](state const& st) {
        return midi_device_name(st.midi_devices, device_id);
    };
}

static bool
is_midi_device_enabled(
        box<std::unordered_map<midi::device_id_t, midi_device_config>> const&
                midi_devices,
        midi::device_id_t const device_id)
{
    auto it = midi_devices->find(device_id);
    return it != midi_devices->end() && it->second.enabled;
}

auto
make_midi_device_enabled_selector(midi::device_id_t const device_id)
        -> selector<bool>
{
    return [device_id](state const& st) {
        return is_midi_device_enabled(st.midi_devices, device_id);
    };
}

using muted_by_solo_state_t = std::
        unordered_map<mixer::channel_id, std::function<bool(state const&)>>;

static auto
make_muted_by_solo_state(mixer::channels_t const& channels)
{
    muted_by_solo_state_t result;

    for (auto&& [owner, group] : runtime::solo_groups(channels))
    {
        auto group_selectors = algorithm::transform_to_vector(
                group,
                [](solo_group_member const& sg_member) {
                    return make_bool_parameter_value_selector(
                            sg_member.solo_param);
                });

        for (auto&& [param, member] : group)
        {
            result.emplace(
                    member,
                    [group_selectors,
                     member_selector = make_bool_parameter_value_selector(
                             param)](state const& st) {
                        return std::ranges::any_of(
                                       group_selectors,
                                       [&st](auto const& sel) {
                                           return sel.get(st);
                                       }) &&
                               !member_selector.get(st);
                    });
        }
    }

    return result;
}

selector<box<muted_by_solo_state_t>> select_muted_by_solo_state(
        [get = memo(boxify_result(&make_muted_by_solo_state))](
                state const& st) { return get(st.mixer_state.channels); });

auto
make_muted_by_solo_selector(mixer::channel_id const mixer_channel_id)
        -> selector<bool>
{
    return [mixer_channel_id](state const& st) {
        auto const& muted_by_solo = select_muted_by_solo_state.get(st);
        auto it = muted_by_solo->find(mixer_channel_id);
        return it != muted_by_solo->end() && it->second(st);
    };
}

auto
make_fx_chain_selector(mixer::channel_id mixer_channel_id)
        -> selector<box<fx::chain_t>>
{
    return [mixer_channel_id](state const& st) -> box<fx::chain_t> {
        static box<fx::chain_t> s_empty_fx_chain;
        mixer::channel const* const mixer_channel =
                st.mixer_state.channels.find(mixer_channel_id);
        return mixer_channel ? mixer_channel->fx_chain : s_empty_fx_chain;
    };
}

auto
make_fx_module_instance_id_selector(fx::module_id const fx_mod_id)
        -> selector<fx::instance_id>
{
    return [fx_mod_id](state const& st) -> fx::instance_id {
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->fx_instance_id : fx::instance_id();
    };
}

auto
make_fx_module_name_selector(fx::module_id const fx_mod_id)
        -> selector<boxed_string>
{
    return [fx_mod_id](state const& st) -> boxed_string {
        static boxed_string s_empty_name;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->name : s_empty_name;
    };
}

auto
make_fx_module_bus_type_selector(fx::module_id const fx_mod_id)
        -> selector<audio::bus_type>
{
    return [fx_mod_id](state const& st) -> audio::bus_type {
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->bus_type : audio::bus_type::mono;
    };
}

auto
make_fx_module_bypass_selector(fx::module_id const fx_mod_id) -> selector<bool>
{
    return [fx_mod_id](state const& st) -> bool {
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod && fx_mod->bypassed;
    };
}

auto
make_fx_module_parameters_selector(fx::module_id const fx_mod_id)
        -> selector<box<fx::module_parameters>>
{
    return [fx_mod_id](state const& st) -> box<fx::module_parameters> {
        static box<fx::module_parameters> s_empty;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->parameters : s_empty;
    };
}

auto
make_fx_module_can_move_left_selector(
        mixer::channel_id const fx_chain_id,
        fx::module_id const fx_mod_id) -> selector<bool>
{
    return [=](state const& st) -> bool {
        mixer::channel const* const mixer_channel =
                st.mixer_state.channels.find(fx_chain_id);
        return mixer_channel && !mixer_channel->fx_chain->empty() &&
               mixer_channel->fx_chain->front() != fx_mod_id;
    };
}

auto
make_fx_module_can_move_right_selector(
        mixer::channel_id const fx_chain_id,
        fx::module_id const fx_mod_id) -> selector<bool>
{
    return [=](state const& st) -> bool {
        mixer::channel const* const mixer_channel =
                st.mixer_state.channels.find(fx_chain_id);
        return mixer_channel && !mixer_channel->fx_chain->empty() &&
               mixer_channel->fx_chain->back() != fx_mod_id;
    };
}

auto
make_fx_parameter_name_selector(fx::parameter_id const fx_param_id)
        -> selector<boxed_string>
{
    return [fx_param_id](state const& st) -> boxed_string {
        static boxed_string s_empty;
        if (auto it = st.fx_parameters->find(fx_param_id);
            it != st.fx_parameters->end())
        {
            return it->second.name;
        }
        return s_empty;
    };
}

auto
make_fx_parameter_value_string_selector(fx::parameter_id const param_id)
        -> selector<std::string>
{
    return [param_id](state const& st) -> std::string {
        if (auto it = st.fx_parameters->find(param_id);
            it != st.fx_parameters->end())
        {
            return std::visit(
                    [it, &st](auto&& id) {
                        return it->second.value_to_string(
                                get_parameter_value(st.params, id));
                    },
                    param_id);
        }
        return {};
    };
}

auto
make_fx_parameter_value_string_selector(
        fx::parameter_id const param_id,
        int const value) -> selector<std::string>
{
    return [=](state const& st) -> std::string {
        if (auto it = st.fx_parameters->find(param_id);
            it != st.fx_parameters->end())
        {
            return it->second.value_to_string(value);
        }
        return std::to_string(value);
    };
}

auto
make_fx_module_streams_selector(fx::module_id fx_mod_id)
        -> selector<box<fx::module_streams>>
{
    return [fx_mod_id](state const& st) -> box<fx::module_streams> {
        static box<fx::module_streams> s_empty;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->streams : s_empty;
    };
}

auto
make_audio_stream_selector(audio_stream_id stream_id)
        -> selector<audio_stream_buffer>
{
    return [stream_id](state const& st) -> audio_stream_buffer {
        static audio_stream_buffer s_empty(std::in_place, 1);
        auto* stream = st.streams.find(stream_id);
        return stream ? *stream : s_empty;
    };
}

auto
make_bool_parameter_value_selector(bool_parameter_id const param_id)
        -> selector<bool>
{
    return [param_id, value = std::shared_ptr<bool const>()](
                   state const& st) mutable -> bool {
        if (value) [[likely]]
        {
            return *value;
        }

        value = get_parameter_cached_value(st.params, param_id);
        return value && *value;
    };
}

auto
make_float_parameter_value_selector(float_parameter_id const param_id)
        -> selector<float>
{
    return [param_id, value = std::shared_ptr<float const>()](
                   state const& st) mutable -> float {
        if (value) [[likely]]
        {
            return *value;
        }

        value = get_parameter_cached_value(st.params, param_id);
        return value ? *value : float{};
    };
}

auto
make_float_parameter_normalized_value_selector(
        float_parameter_id const param_id) -> selector<float>
{
    return [param_id](state const& st) -> float {
        if (float_parameter const* const param =
                    find_parameter(st.params, param_id))
        {
            float const value = get_parameter_value(st.params, param_id);
            BOOST_ASSERT(param->to_normalized);
            return param->to_normalized(*param, value);
        }
        return {};
    };
}

auto
make_int_parameter_value_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id, value = std::shared_ptr<int const>()](
                   state const& st) mutable -> int {
        if (value) [[likely]]
        {
            return *value;
        }

        value = get_parameter_cached_value(st.params, param_id);
        return value ? *value : int{};
    };
}

auto
make_int_parameter_min_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const param = find_parameter(st.params, param_id);
        return param ? param->min : 0;
    };
}

auto
make_int_parameter_max_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const param = find_parameter(st.params, param_id);
        return param ? param->max : 0;
    };
}

auto
make_level_parameter_value_selector(stereo_level_parameter_id const param_id)
        -> selector<stereo_level>
{
    return [param_id, value = std::shared_ptr<stereo_level const>()](
                   state const& st) mutable -> stereo_level {
        if (value) [[likely]]
        {
            return *value;
        }

        value = get_parameter_cached_value(st.params, param_id);
        return value ? *value : stereo_level{};
    };
}

auto
make_midi_assignment_selector(midi_assignment_id const id)
        -> selector<std::optional<midi_assignment>>
{
    return [id](state const& st) -> std::optional<midi_assignment> {
        auto it = st.midi_assignments->find(id);
        return it != st.midi_assignments->end()
                       ? it->second
                       : std::optional<midi_assignment>{};
    };
}

const selector<bool> select_midi_learning([](state const& st) {
    return st.midi_learning.has_value();
});

const selector<fx::registry> select_fx_registry([](state const& st) {
    return st.fx_registry;
});

const selector<bool> select_recording([](state const& st) {
    return st.recording;
});

const selector<std::size_t> select_xruns([](state const& st) {
    return st.xruns;
});

const selector<float> select_cpu_load([](state const& st) {
    return st.cpu_load;
});

} // namespace piejam::runtime::selectors
