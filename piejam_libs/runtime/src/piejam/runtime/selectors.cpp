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
#include <piejam/runtime/state.h>

#include <cassert>

namespace piejam::runtime::selectors
{

const selector<samplerate> select_samplerate(
        [get_samplerates = memo(boxify_result(&runtime::samplerates))](
                state const& st) mutable -> samplerate {
            return {get_samplerates(st.input.hw_params, st.output.hw_params),
                    st.samplerate};
        });

const selector<period_size> select_period_size(
        [get_period_sizes = memo(boxify_result(&runtime::period_sizes))](
                state const& st) mutable -> period_size {
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
make_bus_list_selector(io_direction const bd)
        -> selector<box<mixer::bus_list_t>>
{
    switch (bd)
    {
        case io_direction::input:
            return [](state const& st) { return st.mixer_state.inputs; };

        case io_direction::output:
            return [](state const& st) { return st.mixer_state.outputs; };
    }
}

static auto
make_bus_infos(
        mixer::buses_t const& buses,
        box<mixer::bus_list_t> const& bus_ids) -> boxed_vector<mixer_bus_info>
{
    return algorithm::transform_to_vector(*bus_ids, [&buses](auto&& bus_id) {
        auto bus = buses[bus_id];
        BOOST_ASSERT(bus);
        return mixer_bus_info{
                .bus_id = bus_id,
                .volume = bus->volume,
                .pan_balance = bus->pan_balance,
                .mute = bus->mute,
                .level = bus->level};
    });
}

auto
make_bus_infos_selector(io_direction const bd)
        -> selector<boxed_vector<mixer_bus_info>>
{
    switch (bd)
    {
        case io_direction::input:
            return [get_infos =
                            memo(&make_bus_infos)](state const& st) mutable {
                return get_infos(st.mixer_state.buses, st.mixer_state.inputs);
            };

        case io_direction::output:
            return [get_infos =
                            memo(&make_bus_infos)](state const& st) mutable {
                return get_infos(st.mixer_state.buses, st.mixer_state.outputs);
            };
    }
}

auto
make_bus_name_selector(mixer::bus_id bus_id) -> selector<boxed_string>
{
    return [bus_id](state const& st) mutable -> boxed_string {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];
        return bus ? bus->name : boxed_string();
    };
}

auto
make_bus_type_selector(mixer::bus_id const bus_id) -> selector<audio::bus_type>
{
    return [bus_id](state const& st) -> audio::bus_type {
        mixer::bus const* bus = st.mixer_state.buses[bus_id];
        return bus ? bus->type : audio::bus_type::mono;
    };
}

auto
make_bus_channel_selector(
        mixer::bus_id const bus_id,
        audio::bus_channel const bc) -> selector<std::size_t>
{
    switch (bc)
    {
        case audio::bus_channel::mono:
        case audio::bus_channel::left:
            return [bus_id](state const& st) -> std::size_t {
                mixer::bus const* const bus = st.mixer_state.buses[bus_id];
                return bus ? bus->device_channels.left : piejam::npos;
            };

        case audio::bus_channel::right:
            return [bus_id](state const& st) -> std::size_t {
                mixer::bus const* const bus = st.mixer_state.buses[bus_id];
                return bus ? bus->device_channels.right : piejam::npos;
            };
    }

    BOOST_ASSERT(false);
    __builtin_unreachable();
}

auto
make_input_solo_selector(mixer::bus_id const bus_id) -> selector<bool>
{
    return [bus_id](state const& st) -> bool {
        return bus_id == st.mixer_state.input_solo_id;
    };
}

auto
make_bus_level_selector(mixer::bus_id const bus_id) -> selector<stereo_level>
{
    return [bus_id](state const& st) -> stereo_level {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];
        stereo_level const* const level =
                bus ? st.params.get(bus->level) : nullptr;
        return level ? *level : stereo_level{};
    };
}

const selector<bool> select_input_solo_active([](state const& st) {
    return st.mixer_state.input_solo_id != mixer::bus_id{};
});

const selector<mixer::bus_id> select_fx_chain_bus([](state const& st) {
    return st.fx_chain_bus;
});

const selector<box<fx::chain_t>> select_current_fx_chain([](state const& st) {
    static box<fx::chain_t> s_empty_fx_chain;
    mixer::bus const* const bus = st.mixer_state.buses[st.fx_chain_bus];
    return bus ? bus->fx_chain : s_empty_fx_chain;
});

const selector<stereo_level>
        select_current_fx_chain_bus_level([](state const& st) -> stereo_level {
            mixer::bus const* const bus = st.mixer_state.buses[st.fx_chain_bus];
            stereo_level const* const level =
                    bus ? st.params.get(bus->level) : nullptr;
            return level ? *level : stereo_level();
        });

const selector<float>
select_current_fx_chain_bus_volume([](state const& st) -> float {
    mixer::bus const* const bus = st.mixer_state.buses[st.fx_chain_bus];
    float const* const volume = bus ? st.params.get(bus->volume) : nullptr;
    return volume ? *volume : 0.f;
});

auto
make_fx_module_name_selector(fx::module_id fx_mod_id) -> selector<boxed_string>
{
    return [fx_mod_id](state const& st) -> boxed_string {
        static boxed_string s_empty_name;
        fx::module const* const fx_mod = st.fx_modules[fx_mod_id];
        return fx_mod ? fx_mod->name : s_empty_name;
    };
}

auto
make_fx_module_parameters_selector(fx::module_id fx_mod_id)
        -> selector<box<fx::module_parameters>>
{
    return [fx_mod_id](state const& st) -> box<fx::module_parameters> {
        static box<fx::module_parameters> s_empty;
        fx::module const* const fx_mod = st.fx_modules[fx_mod_id];
        return fx_mod ? fx_mod->parameters : s_empty;
    };
}

auto
make_fx_module_can_move_left_selector(fx::module_id fx_mod_id) -> selector<bool>
{
    return [fx_mod_id](state const& st) -> bool {
        mixer::bus const* const bus = st.mixer_state.buses[st.fx_chain_bus];
        return bus && !bus->fx_chain->empty() &&
               bus->fx_chain->front() != fx_mod_id;
    };
}

auto
make_fx_module_can_move_right_selector(fx::module_id fx_mod_id)
        -> selector<bool>
{
    return [fx_mod_id](state const& st) -> bool {
        mixer::bus const* const bus = st.mixer_state.buses[st.fx_chain_bus];
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
        if (fx::module const* const fx_mod = st.fx_modules[fx_mod_id])
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
                        return it->second.value_to_string(*st.params.get(id));
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
    return [param_id](state const& st) -> bool {
        bool const* const value = st.params.get(param_id);
        return value && *value;
    };
}

auto
make_float_parameter_value_selector(float_parameter_id const param_id)
        -> selector<float>
{
    return [param_id](state const& st) -> float {
        float const* const value = st.params.get(param_id);
        return value ? *value : 0.f;
    };
}

auto
make_float_parameter_normalized_value_selector(
        float_parameter_id const param_id) -> selector<float>
{
    return [param_id](state const& st) -> float {
        if (float_parameter const* const param =
                    st.params.get_parameter(param_id))
        {
            float const* const value = st.params.get(param_id);
            BOOST_ASSERT(value);
            BOOST_ASSERT(param->to_normalized);
            return param->to_normalized(*param, *value);
        }
        return {};
    };
}

auto
make_int_parameter_value_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        int const* const value = st.params.get(param_id);
        return value ? *value : 0;
    };
}

auto
make_int_parameter_min_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const param = st.params.get_parameter(param_id);
        return param ? param->min : 0;
    };
}

auto
make_int_parameter_max_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const param = st.params.get_parameter(param_id);
        return param ? param->max : 0;
    };
}

auto
make_level_parameter_value_selector(stereo_level_parameter_id const param_id)
        -> selector<stereo_level>
{
    return [param_id](state const& st) -> stereo_level {
        stereo_level const* const value = st.params.get(param_id);
        return value ? *value : stereo_level{};
    };
}

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
