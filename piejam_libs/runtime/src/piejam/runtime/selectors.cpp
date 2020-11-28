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

#include <piejam/runtime/selectors.h>

#include <piejam/container/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/npos.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/audio_state.h>

#include <cassert>

namespace piejam::runtime::selectors
{

const selector<samplerate> select_samplerate(
        [get_samplerates =
                 memoized(container::boxify_result(&runtime::samplerates))](
                audio_state const& st) mutable -> samplerate {
            return {get_samplerates(st.input.hw_params, st.output.hw_params),
                    st.samplerate};
        });

const selector<period_size> select_period_size(
        [get_period_sizes =
                 memoized(container::boxify_result(&runtime::period_sizes))](
                audio_state const& st) mutable -> period_size {
            return {get_period_sizes(st.input.hw_params, st.output.hw_params),
                    st.period_size};
        });

const selector<input_devices>
        select_input_devices([](audio_state const& st) -> input_devices {
            return {st.pcm_devices, st.input.index};
        });

const selector<output_devices>
        select_output_devices([](audio_state const& st) -> output_devices {
            return {st.pcm_devices, st.output.index};
        });

auto
make_num_device_channels_selector(audio::bus_direction const bd)
        -> selector<std::size_t>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return selector<std::size_t>(
                    [](audio_state const& st) -> std::size_t {
                        return st.input.hw_params->num_channels;
                    });

        case audio::bus_direction::output:
            return selector<std::size_t>(
                    [](audio_state const& st) -> std::size_t {
                        return st.output.hw_params->num_channels;
                    });
    }
}

auto
make_bus_list_selector(audio::bus_direction const bd)
        -> selector<container::box<mixer::bus_list_t>>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return [](audio_state const& st) { return st.mixer_state.inputs; };

        case audio::bus_direction::output:
            return [](audio_state const& st) { return st.mixer_state.outputs; };
    }
}

auto
make_bus_name_selector(mixer::bus_id bus_id)
        -> selector<container::boxed_string>
{
    return [bus_id](audio_state const& st) -> container::boxed_string {
        if (mixer::bus const* bus = st.mixer_state.buses[bus_id])
        {
            return bus->name;
        }

        return {};
    };
}

auto
make_bus_type_selector(mixer::bus_id const bus_id) -> selector<audio::bus_type>
{
    return [bus_id](audio_state const& st) -> audio::bus_type {
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
            return [bus_id](audio_state const& st) -> std::size_t {
                mixer::bus const* const bus = st.mixer_state.buses[bus_id];
                return bus ? bus->device_channels.left : piejam::npos;
            };

        case audio::bus_channel::right:
            return [bus_id](audio_state const& st) -> std::size_t {
                mixer::bus const* const bus = st.mixer_state.buses[bus_id];
                return bus ? bus->device_channels.right : piejam::npos;
            };
    }

    BOOST_ASSERT(false);
    __builtin_unreachable();
}

auto
make_bus_volume_selector(mixer::bus_id bus_id) -> selector<float>
{
    return [bus_id](audio_state const& st) -> float {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];
        float const* const volume =
                bus ? st.float_params.get(bus->volume) : nullptr;
        return volume ? *volume : 1.f;
    };
}

auto
make_bus_pan_balance_selector(mixer::bus_id bus_id) -> selector<float>
{
    return [bus_id](audio_state const& st) -> float {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];
        float const* const pan_balance =
                bus ? st.float_params.get(bus->pan_balance) : nullptr;
        return pan_balance ? *pan_balance : 0.f;
    };
}

auto
make_bus_mute_selector(mixer::bus_id const bus_id) -> selector<bool>
{
    return [bus_id](audio_state const& st) -> bool {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];
        bool const* const mute = bus ? st.bool_params.get(bus->mute) : nullptr;
        return mute && *mute;
    };
}

auto
make_input_solo_selector(mixer::bus_id const bus_id) -> selector<bool>
{
    return [bus_id](audio_state const& st) -> bool {
        return bus_id == st.mixer_state.input_solo_id;
    };
}

auto
make_bus_level_selector(mixer::bus_id const bus_id) -> selector<stereo_level>
{
    return [bus_id](audio_state const& st) -> stereo_level {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];
        stereo_level const* const level =
                bus ? st.levels.get(bus->level) : nullptr;
        return level ? *level : stereo_level{};
    };
}

const selector<bool> select_input_solo_active([](audio_state const& st) {
    return st.mixer_state.input_solo_id != mixer::bus_id{};
});

const selector<mixer::bus_id> select_fx_chain_bus([](audio_state const& st) {
    return st.fx_chain_bus;
});

const selector<container::box<fx::chain_t>>
        select_current_fx_chain([](audio_state const& st) {
            static container::box<fx::chain_t> s_empty_fx_chain;
            mixer::bus const* const bus = st.mixer_state.buses[st.fx_chain_bus];
            return bus ? bus->fx_chain : s_empty_fx_chain;
        });

auto
make_fx_module_name_selector(fx::module_id fx_mod_id)
        -> selector<container::boxed_string>
{
    return [fx_mod_id](audio_state const& st) -> container::boxed_string {
        static container::boxed_string s_empty_name;
        fx::module const* const fx_mod = st.fx_modules[fx_mod_id];
        return fx_mod ? fx_mod->name : s_empty_name;
    };
}

auto
make_fx_module_parameters_selector(fx::module_id fx_mod_id)
        -> selector<container::box<fx::parameters_t>>
{
    return [fx_mod_id](
                   audio_state const& st) -> container::box<fx::parameters_t> {
        static container::box<fx::parameters_t> s_empty;
        fx::module const* const fx_mod = st.fx_modules[fx_mod_id];
        return fx_mod ? fx_mod->parameters : s_empty;
    };
}

auto
make_fx_parameter_name_selector(
        fx::module_id const fx_mod_id,
        fx::parameter_key const fx_param_key)
        -> selector<container::boxed_string>
{
    return [fx_mod_id,
            fx_param_key](audio_state const& st) -> container::boxed_string {
        static container::boxed_string s_empty;
        if (fx::module const* const fx_mod = st.fx_modules[fx_mod_id])
        {
            if (auto it = fx_mod->parameters->find(fx_param_key);
                it != fx_mod->parameters->end())
            {
                return it->second.name;
            }
        }
        return s_empty;
    };
}

auto
make_fx_parameter_id_selector(
        fx::module_id const fx_mod_id,
        fx::parameter_key const fx_param_key) -> selector<float_parameter_id>
{
    return [fx_mod_id,
            fx_param_key](audio_state const& st) -> float_parameter_id {
        if (fx::module const* const fx_mod = st.fx_modules[fx_mod_id])
        {
            if (auto it = fx_mod->parameters->find(fx_param_key);
                it != fx_mod->parameters->end())
            {
                return it->second.id;
            }
        }
        return {};
    };
}

const selector<std::size_t> select_xruns([](audio_state const& st) {
    return st.xruns;
});

const selector<float> select_cpu_load([](audio_state const& st) {
    return st.cpu_load;
});

} // namespace piejam::runtime::selectors
