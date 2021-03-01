// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/selectors.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/functional/overload.h>
#include <piejam/npos.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/mixer_util.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/state.h>

#include <cassert>

namespace piejam::runtime::selectors
{

const selector<samplerate> select_samplerate(
        [get_samplerates = memo(boxify_result(&runtime::samplerates))](
                state const& st) -> samplerate {
            return {get_samplerates(st.input.hw_params, st.output.hw_params),
                    st.samplerate};
        });

const selector<period_size> select_period_size(
        [get_period_sizes = memo(boxify_result(&runtime::period_sizes))](
                state const& st) -> period_size {
            return {get_period_sizes(st.input.hw_params, st.output.hw_params),
                    st.period_size};
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

static auto
make_bus_info(mixer::buses_t const& buses, mixer::bus_id const bus_id)
        -> mixer_bus_info
{
    mixer::bus const& bus = buses[bus_id];
    return mixer_bus_info{
            .bus_id = bus_id,
            .volume = bus.volume,
            .pan_balance = bus.pan_balance,
            .mute = bus.mute,
            .level = bus.level};
}

static auto
make_bus_infos(
        mixer::buses_t const& buses,
        box<mixer::bus_list_t> const& bus_ids) -> boxed_vector<mixer_bus_info>
{
    return algorithm::transform_to_vector(*bus_ids, [&buses](auto&& bus_id) {
        return make_bus_info(buses, bus_id);
    });
}

const selector<boxed_vector<mixer_bus_info>> select_mixer_bus_infos(
        [get_infos = memo(&make_bus_infos)](state const& st) {
            return get_infos(st.mixer_state.buses, st.mixer_state.inputs);
        });

const selector<box<mixer_bus_info>> select_mixer_main_bus_info(
        [get = memo(boxify_result(&make_bus_info))](state const& st) {
            return get(st.mixer_state.buses, st.mixer_state.main);
        });

static auto
make_mixer_device_routes(
        device_io::buses_t const& device_buses,
        box<device_io::bus_list_t> const& inputs)
{
    return algorithm::transform_to_vector(
            *inputs,
            [&](device_io::bus_id bus_id) {
                device_io::bus const* const bus = device_buses.find(bus_id);
                return mixer_device_route{
                        .bus_id = bus_id,
                        .name = bus ? *bus->name : std::string()};
            });
}

const selector<boxed_vector<mixer_device_route>> select_mixer_input_devices(
        [get = memo(boxify_result(&make_mixer_device_routes))](
                state const& st) mutable {
            return get(st.device_io_state.buses, st.device_io_state.inputs);
        });

const selector<boxed_vector<mixer_device_route>> select_mixer_output_devices(
        [get = memo(boxify_result(&make_mixer_device_routes))](
                state const& st) mutable {
            return get(st.device_io_state.buses, st.device_io_state.outputs);
        });

auto
make_default_mixer_channel_input_is_valid_selector(mixer::bus_id const bus_id)
        -> selector<bool>
{
    return [bus_id,
            get = memo(&mixer::is_default_source_valid)](state const& st) {
        return get(st.mixer_state.buses, bus_id);
    };
}

static auto
make_mixer_input_channel_routes(
        mixer::buses_t const& mixer_buses,
        mixer::bus_id const bus_id)
{
    auto valid_sources = mixer::valid_source_channels(mixer_buses, bus_id);
    return algorithm::transform_to_vector(valid_sources, [&](auto const& id) {
        return mixer_channel_route{.bus_id = id, .name = mixer_buses[id].name};
    });
}

auto
make_mixer_input_channels_selector(mixer::bus_id const bus_id)
        -> selector<boxed_vector<mixer_channel_route>>
{
    return [bus_id,
            get = memo(boxify_result(&make_mixer_input_channel_routes))](
                   state const& st) {
        return get(st.mixer_state.buses, bus_id);
    };
}

static auto
make_mixer_output_channel_routes(
        mixer::buses_t const& mixer_buses,
        mixer::bus_id const bus_id)
{
    auto valid_targets = mixer::valid_target_channels(mixer_buses, bus_id);
    return algorithm::transform_to_vector(valid_targets, [&](auto const& id) {
        return mixer_channel_route{.bus_id = id, .name = mixer_buses[id].name};
    });
}

auto
make_mixer_output_channels_selector(mixer::bus_id const bus_id)
        -> selector<boxed_vector<mixer_channel_route>>
{
    return [bus_id,
            get = memo(boxify_result(&make_mixer_output_channel_routes))](
                   state const& st) {
        return get(st.mixer_state.buses, bus_id);
    };
}

static auto
get_mixer_channel_selected_input(
        mixer::bus_id const bus_id,
        mixer::buses_t const& mixer_buses,
        device_io::buses_t const& device_buses) -> selected_route
{
    static std::string s_mix{"Mix"};

    mixer::bus const* const bus = mixer_buses.find(bus_id);
    if (!bus)
        return selected_route{selected_route::state_t::invalid, s_mix};

    return std::visit(
            overload{
                    [](std::nullptr_t) {
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = s_mix};
                    },
                    [&](mixer::bus_id const src_bus_id) {
                        mixer::bus const* const src_bus =
                                mixer_buses.find(src_bus_id);
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
                    }},
            bus->in);
}

auto
make_mixer_channel_input_selector(mixer::bus_id const bus_id)
        -> selector<box<selected_route>>
{
    return [bus_id,
            get = memo(boxify_result(&get_mixer_channel_selected_input))](
                   state const& st) {
        return get(bus_id, st.mixer_state.buses, st.device_io_state.buses);
    };
}

static auto
get_mixer_channel_selected_output(
        mixer::bus_id const bus_id,
        mixer::buses_t const& mixer_buses,
        device_io::buses_t const& device_buses) -> selected_route
{
    static std::string s_none{"None"};

    mixer::bus const* const bus = mixer_buses.find(bus_id);
    if (!bus)
        return selected_route{selected_route::state_t::invalid, s_none};

    return std::visit(
            overload{
                    [](std::nullptr_t) {
                        return selected_route{
                                .state = selected_route::state_t::valid,
                                .name = s_none};
                    },
                    [&](mixer::bus_id const dst_bus_id) {
                        if (mixer::bus const* const dst_bus =
                                    mixer_buses.find(dst_bus_id))
                        {
                            return selected_route{
                                    .state =
                                            std::holds_alternative<
                                                    std::nullptr_t>(dst_bus->in)
                                                    ? selected_route::state_t::
                                                              valid
                                                    : selected_route::state_t::
                                                              not_mixed,
                                    .name = dst_bus->name};
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
                    }},
            bus->out);
}

auto
make_mixer_channel_output_selector(mixer::bus_id const bus_id)
        -> selector<box<selected_route>>
{
    return [bus_id,
            get = memo(boxify_result(&get_mixer_channel_selected_output))](
                   state const& st) {
        return get(bus_id, st.mixer_state.buses, st.device_io_state.buses);
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
make_mixer_bus_name_selector(mixer::bus_id const bus_id)
        -> selector<boxed_string>
{
    auto get_mixer_bus_name = [](mixer::buses_t const& mixer_buses,
                                 mixer::bus_id const bus_id) {
        mixer::bus const* const bus = mixer_buses.find(bus_id);
        return bus ? bus->name : boxed_string();
    };

    return [bus_id,
            get = memo(get_mixer_bus_name)](state const& st) -> boxed_string {
        return get(st.mixer_state.buses, bus_id);
    };
}

auto
make_mixer_bus_input_type_selector(mixer::bus_id const bus_id)
        -> selector<audio::bus_type>
{
    return [bus_id, get = memo(&mixer_bus_input_type)](state const& st) {
        return get(st.mixer_state.buses, bus_id, st.device_io_state.buses);
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

auto
make_solo_selector(io_direction io_dir, mixer::bus_id const bus_id)
        -> selector<bool>
{
    switch (io_dir)
    {
        case io_direction::input:
            return [bus_id](state const& st) -> bool {
                return bus_id == st.mixer_state.input_solo_id;
            };

        case io_direction::output:
            return [bus_id](state const& st) -> bool {
                return bus_id == st.mixer_state.output_solo_id;
            };
    }
}

const selector<bool> select_input_solo_active([](state const& st) {
    return st.mixer_state.input_solo_id.valid();
});

const selector<bool> select_output_solo_active([](state const& st) {
    return st.mixer_state.output_solo_id.valid();
});

const selector<mixer::bus_id> select_fx_chain_bus([](state const& st) {
    return st.fx_chain_bus;
});

const selector<box<fx::chain_t>> select_current_fx_chain([](state const& st) {
    static box<fx::chain_t> s_empty_fx_chain;
    mixer::bus const* const bus = st.mixer_state.buses.find(st.fx_chain_bus);
    return bus ? bus->fx_chain : s_empty_fx_chain;
});

const selector<stereo_level>
        select_current_fx_chain_bus_level([](state const& st) -> stereo_level {
            mixer::bus const* const bus =
                    st.mixer_state.buses.find(st.fx_chain_bus);
            stereo_level const* const level =
                    bus ? find_parameter_value(st.params, bus->level) : nullptr;
            return level ? *level : stereo_level();
        });

const selector<float>
select_current_fx_chain_bus_volume([](state const& st) -> float {
    mixer::bus const* const bus = st.mixer_state.buses.find(st.fx_chain_bus);
    float const* const volume =
            bus ? find_parameter_value(st.params, bus->volume) : nullptr;
    return volume ? *volume : 0.f;
});

auto
make_fx_module_name_selector(fx::module_id fx_mod_id) -> selector<boxed_string>
{
    return [fx_mod_id](state const& st) -> boxed_string {
        static boxed_string s_empty_name;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->name : s_empty_name;
    };
}

auto
make_fx_module_parameters_selector(fx::module_id fx_mod_id)
        -> selector<box<fx::module_parameters>>
{
    return [fx_mod_id](state const& st) -> box<fx::module_parameters> {
        static box<fx::module_parameters> s_empty;
        fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id);
        return fx_mod ? fx_mod->parameters : s_empty;
    };
}

auto
make_fx_module_can_move_left_selector(fx::module_id fx_mod_id) -> selector<bool>
{
    return [fx_mod_id](state const& st) -> bool {
        mixer::bus const* const bus =
                st.mixer_state.buses.find(st.fx_chain_bus);
        return bus && !bus->fx_chain->empty() &&
               bus->fx_chain->front() != fx_mod_id;
    };
}

auto
make_fx_module_can_move_right_selector(fx::module_id fx_mod_id)
        -> selector<bool>
{
    return [fx_mod_id](state const& st) -> bool {
        mixer::bus const* const bus =
                st.mixer_state.buses.find(st.fx_chain_bus);
        return bus && !bus->fx_chain->empty() &&
               bus->fx_chain->back() != fx_mod_id;
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
make_fx_parameter_id_selector(
        fx::module_id const fx_mod_id,
        fx::parameter_key const fx_param_key) -> selector<fx::parameter_id>
{
    return [fx_mod_id, fx_param_key](state const& st) -> fx::parameter_id {
        if (fx::module const* const fx_mod = st.fx_modules.find(fx_mod_id))
        {
            if (auto it = fx_mod->parameters->find(fx_param_key);
                it != fx_mod->parameters->end())
            {
                return it->second;
            }
        }
        return {};
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
make_bool_parameter_value_selector(bool_parameter_id const param_id)
        -> selector<bool>
{
    return [param_id, value = std::shared_ptr<bool const>()](
                   state const& st) mutable -> bool {
        if (value) [[likely]]
            return *value;

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
            return *value;

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
            return *value;

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
            return *value;

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

const selector<std::size_t> select_xruns([](state const& st) {
    return st.xruns;
});

const selector<float> select_cpu_load([](state const& st) {
    return st.cpu_load;
});

} // namespace piejam::runtime::selectors
