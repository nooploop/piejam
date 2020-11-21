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

#include <piejam/runtime/audio_state_selectors.h>

#include <piejam/container/boxify_result.h>
#include <piejam/functional/memo.h>
#include <piejam/npos.h>
#include <piejam/reselect/selector.h>
#include <piejam/runtime/audio_state.h>

#include <cassert>

namespace piejam::runtime::audio_state_selectors
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

template <audio::bus_direction D>
static auto
make_bus_name_selector(std::size_t const bus)
        -> selector<container::boxed_string>
{
    return [bus](audio_state const& st) -> container::boxed_string {
        return bus < mixer::bus_ids<D>(st.mixer_state).size()
                       ? mixer::get_bus<D>(st.mixer_state, bus).name
                       : container::boxed_string();
    };
}

auto
make_bus_name_selector(audio::bus_direction const bd, std::size_t const bus)
        -> selector<container::boxed_string>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return make_bus_name_selector<audio::bus_direction::input>(bus);

        case audio::bus_direction::output:
            return make_bus_name_selector<audio::bus_direction::output>(bus);
    }
}

template <audio::bus_direction D>
static auto
make_bus_type_selector(std::size_t const bus, audio::bus_type const def)
{
    return [bus, def](audio_state const& st) -> audio::bus_type {
        return bus < mixer::bus_ids<D>(st.mixer_state).size()
                       ? mixer::get_bus<D>(st.mixer_state, bus).type
                       : def;
    };
}

auto
make_bus_type_selector(audio::bus_direction const bd, std::size_t const bus)
        -> selector<audio::bus_type>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return make_bus_type_selector<audio::bus_direction::input>(
                    bus,
                    audio::bus_type::mono);

        case audio::bus_direction::output:
            return make_bus_type_selector<audio::bus_direction::output>(
                    bus,
                    audio::bus_type::stereo);
    }
}

template <audio::bus_direction D>
static auto
make_bus_channel_selector(std::size_t const bus, audio::bus_channel const bc)
        -> selector<std::size_t>
{
    switch (bc)
    {
        case audio::bus_channel::mono:
        case audio::bus_channel::left:
            return [bus](audio_state const& st) -> std::size_t {
                return bus < mixer::bus_ids<D>(st.mixer_state).size()
                               ? mixer::get_bus<D>(st.mixer_state, bus)
                                         .device_channels.left
                               : piejam::npos;
            };

        case audio::bus_channel::right:
            return [bus](audio_state const& st) -> std::size_t {
                return bus < mixer::bus_ids<D>(st.mixer_state).size()
                               ? mixer::get_bus<D>(st.mixer_state, bus)
                                         .device_channels.right
                               : piejam::npos;
            };
    }

    throw;
}

auto
make_bus_channel_selector(
        audio::bus_direction const bd,
        std::size_t const bus,
        audio::bus_channel const bc) -> selector<std::size_t>
{
    switch (bd)
    {
        case audio::bus_direction::input:
            return make_bus_channel_selector<audio::bus_direction::input>(
                    bus,
                    bc);

        case audio::bus_direction::output:
            return make_bus_channel_selector<audio::bus_direction::output>(
                    bus,
                    bc);
    }
}

auto
make_bus_volume_selector(mixer::bus_id bus_id) -> selector<float>
{
    return [bus_id](audio_state const& st) -> float {
        auto it = st.mixer_state.buses.find(bus_id);
        float const* const volume =
                it != st.mixer_state.buses.end()
                        ? st.float_params.get(it->second.volume)
                        : nullptr;
        return volume ? *volume : 1.f;
    };
}

auto
make_bus_pan_balance_selector(mixer::bus_id bus_id) -> selector<float>
{
    return [bus_id](audio_state const& st) -> float {
        auto it = st.mixer_state.buses.find(bus_id);
        float const* const pan_balance =
                it != st.mixer_state.buses.end()
                        ? st.float_params.get(it->second.pan_balance)
                        : nullptr;
        return pan_balance ? *pan_balance : 0.f;
    };
}

auto
make_bus_mute_selector(mixer::bus_id bus_id) -> selector<bool>
{
    return [bus_id](audio_state const& st) -> bool {
        auto it = st.mixer_state.buses.find(bus_id);
        bool const* const mute = it != st.mixer_state.buses.end()
                                         ? st.bool_params.get(it->second.mute)
                                         : nullptr;
        return mute && *mute;
    };
}

auto
make_input_solo_selector(std::size_t const index) -> selector<bool>
{
    return selector<bool>([index](audio_state const& st) -> bool {
        return index < st.mixer_state.inputs->size() &&
               st.mixer_state.inputs.get()[index] ==
                       st.mixer_state.input_solo_id;
    });
}

template <audio::bus_direction D>
static auto
make_level_selector(std::size_t const index) -> selector<stereo_level>
{
    return [index](audio_state const& st) -> stereo_level {
        return index < mixer::bus_ids<D>(st.mixer_state).size()
                       ? mixer::get_bus<D>(st.mixer_state, index).level
                       : stereo_level{};
    };
}

auto
make_input_level_selector(std::size_t const index) -> selector<stereo_level>
{
    return make_level_selector<audio::bus_direction::input>(index);
}

const selector<bool> select_input_solo_active([](audio_state const& st) {
    return st.mixer_state.input_solo_id != mixer::bus_id{};
});

auto
make_output_level_selector(std::size_t const index) -> selector<stereo_level>
{
    return make_level_selector<audio::bus_direction::output>(index);
}

const selector<std::size_t> select_xruns([](audio_state const& st) {
    return st.xruns;
});

const selector<float> select_cpu_load([](audio_state const& st) {
    return st.cpu_load;
});

} // namespace piejam::runtime::audio_state_selectors
