// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/state.h>

#include <piejam/algorithm/contains.h>
#include <piejam/audio/types.h>
#include <piejam/functional/operators.h>
#include <piejam/indexed_access.h>
#include <piejam/ladspa/port_descriptor.h>
#include <piejam/runtime/fader_mapping.h>
#include <piejam/runtime/internal_fx_module_factory.h>
#include <piejam/runtime/ladspa_fx/ladspa_fx_module.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/set_if.h>
#include <piejam/tuple_element_compare.h>

#include <fmt/format.h>

#include <boost/hof/match.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <algorithm>

namespace piejam::runtime
{

namespace
{

constexpr auto
to_normalized_volume(float_parameter const&, float const value)
{
    return parameter::to_normalized_dB_mapping<
            fader_mapping::volume,
            fader_mapping::min_gain_dB>(value);
}

constexpr auto
from_normalized_volume(float_parameter const&, float const norm_value) -> float
{
    return parameter::from_normalized_dB_maping<
            fader_mapping::volume,
            fader_mapping::min_gain_dB>(norm_value);
}

constexpr auto
to_normalized_send(float_parameter const&, float const value)
{
    return parameter::to_normalized_dB_mapping<
            fader_mapping::send,
            fader_mapping::min_gain_dB>(value);
}

constexpr auto
from_normalized_send(float_parameter const&, float const norm_value) -> float
{
    return parameter::from_normalized_dB_maping<
            fader_mapping::send,
            fader_mapping::min_gain_dB>(norm_value);
}

auto
volume_to_string(float volume) -> std::string
{
    auto const volume_dB = math::to_dB(volume);
    return fmt::format("{:.1f} dB", volume_dB);
}

} // namespace

auto
make_initial_state() -> state
{
    state st;
    st.mixer_state.main =
            add_mixer_channel(st, "Main", audio::bus_type::stereo);
    // main doesn't belong into inputs
    remove_erase(st.mixer_state.inputs, st.mixer_state.main);
    // reset the output to default back again
    st.mixer_state.channels.lock()[st.mixer_state.main].out =
            piejam::default_t{};
    st.params[st.mixer_state.channels[st.mixer_state.main].record].value.set(
            true);
    return st;
}

template <class Vector>
static auto
set_intersection(Vector const& in, Vector const& out)
{
    Vector result;
    auto proj = [](auto const& x) { return x.value(); };
    std::ranges::set_intersection(
            in,
            out,
            std::back_inserter(result),
            {},
            proj,
            proj);
    return result;
}

auto
sample_rates(
        box<audio::sound_card_hw_params> const& input_hw_params,
        box<audio::sound_card_hw_params> const& output_hw_params)
        -> audio::sample_rates_t
{
    return set_intersection(
            input_hw_params->sample_rates,
            output_hw_params->sample_rates);
}

auto
sample_rates_from_state(state const& state) -> audio::sample_rates_t
{
    return sample_rates(
            state.selected_io_sound_card.in.hw_params,
            state.selected_io_sound_card.out.hw_params);
}

auto
period_sizes(
        box<audio::sound_card_hw_params> const& input_hw_params,
        box<audio::sound_card_hw_params> const& output_hw_params)
        -> audio::period_sizes_t
{
    return set_intersection(
            input_hw_params->period_sizes,
            output_hw_params->period_sizes);
}

auto
period_sizes_from_state(state const& state) -> audio::period_sizes_t
{
    return period_sizes(
            state.selected_io_sound_card.in.hw_params,
            state.selected_io_sound_card.out.hw_params);
}

auto
period_counts(
        box<audio::sound_card_hw_params> const& input_hw_params,
        box<audio::sound_card_hw_params> const& output_hw_params)
        -> audio::period_counts_t
{
    return set_intersection(
            input_hw_params->period_counts,
            output_hw_params->period_counts);
}

auto
period_counts_from_state(state const& state) -> audio::period_counts_t
{
    return period_counts(
            state.selected_io_sound_card.in.hw_params,
            state.selected_io_sound_card.out.hw_params);
}

static auto
make_internal_fx_module(fx::modules_t& fx_modules, fx::module&& fx_mod)
{
    return fx_modules.emplace(std::move(fx_mod));
}

void
apply_parameter_values(
        std::vector<fx::parameter_value_assignment> const& values,
        fx::module const& fx_mod,
        parameters_map& params)
{
    for (auto&& [key, value] : values)
    {
        if (auto it = fx_mod.parameters->find(key);
            it != fx_mod.parameters->end())
        {
            auto const& fx_param_id = it->second;
            std::visit(
                    boost::hof::match(
                            [&params]<class P>(
                                    parameter::id_t<P> id,
                                    parameter::value_type_t<P> v) {
                                params[id].value.set(v);
                            },
                            [](auto&&, auto&&) { BOOST_ASSERT(false); }),
                    fx_param_id,
                    value);
        }
    }
}

static void
update_midi_assignments(
        midi_assignments_map& midi_assigns,
        midi_assignments_map const& new_assignments)
{
    for (auto&& [id, ass] : new_assignments)
    {
        std::erase_if(midi_assigns, tuple::element<1>.equal_to(std::cref(ass)));

        midi_assigns.insert_or_assign(id, ass);
    }
}

void
apply_fx_midi_assignments(
        std::vector<fx::parameter_midi_assignment> const& fx_midi_assigns,
        fx::module const& fx_mod,
        midi_assignments_map& midi_assigns)
{
    midi_assignments_map new_assignments;
    for (auto&& [key, value] : fx_midi_assigns)
    {
        if (auto it = fx_mod.parameters->find(key);
            it != fx_mod.parameters->end())
        {
            BOOST_ASSERT(std::visit(
                    []<class ParamId>(ParamId) {
                        return is_midi_assignable_v<ParamId>;
                    },
                    it->second));
            new_assignments.emplace(it->second, value);
        }
    }

    update_midi_assignments(midi_assigns, new_assignments);
}

auto
insert_internal_fx_module(
        state& st,
        mixer::channel_id const mixer_channel_id,
        std::size_t const position,
        fx::internal_id fx_internal_id,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns)
        -> fx::module_id
{
    BOOST_ASSERT(mixer_channel_id.valid());

    mixer::channel const& mixer_channel =
            st.mixer_state.channels[mixer_channel_id];
    auto const bus_type = mixer_channel.bus_type;
    fx::chain_t fx_chain = mixer_channel.fx_chain;
    auto const insert_pos = std::min(position, fx_chain.size());

    fx::module_id fx_mod_id = make_internal_fx_module(
            st.fx_modules,
            internal_fx_module_factories::lookup(fx_internal_id)({
                    .bus_type = bus_type,
                    .sample_rate = st.sample_rate,
                    .params = st.params,
                    .streams = st.streams,
            }));

    fx_chain.emplace(std::next(fx_chain.begin(), insert_pos), fx_mod_id);

    auto const& fx_mod = st.fx_modules[fx_chain[insert_pos]];

    apply_parameter_values(initial_values, fx_mod, st.params);
    apply_fx_midi_assignments(
            midi_assigns,
            fx_mod,
            *st.midi_assignments.lock());

    st.mixer_state.channels.lock()[mixer_channel_id].fx_chain =
            std::move(fx_chain);

    return fx_mod_id;
}

auto
insert_ladspa_fx_module(
        state& st,
        mixer::channel_id const mixer_channel_id,
        std::size_t const position,
        ladspa::instance_id const instance_id,
        ladspa::plugin_descriptor const& plugin_desc,
        std::span<ladspa::port_descriptor const> const control_inputs,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns)
        -> fx::module_id
{
    BOOST_ASSERT(mixer_channel_id != mixer::channel_id{});

    mixer::channel const& mixer_channel =
            st.mixer_state.channels[mixer_channel_id];
    auto const bus_type = mixer_channel.bus_type;
    fx::chain_t fx_chain = mixer_channel.fx_chain;
    auto const insert_pos = std::min(position, fx_chain.size());

    auto fx_mod_id = st.fx_modules.emplace(ladspa_fx::make_module(
            instance_id,
            plugin_desc.name,
            bus_type,
            control_inputs,
            st.params));

    fx_chain.emplace(std::next(fx_chain.begin(), insert_pos), fx_mod_id);

    auto const& fx_mod = st.fx_modules[fx_chain[insert_pos]];
    apply_parameter_values(initial_values, fx_mod, st.params);
    apply_fx_midi_assignments(
            midi_assigns,
            fx_mod,
            *st.midi_assignments.lock());

    st.mixer_state.channels.lock()[mixer_channel_id].fx_chain =
            std::move(fx_chain);

    st.fx_ladspa_instances.emplace(instance_id, plugin_desc);

    return fx_mod_id;
}

void
insert_missing_ladspa_fx_module(
        state& st,
        mixer::channel_id const channel_id,
        std::size_t const position,
        fx::unavailable_ladspa const& unavail,
        std::string_view const name)
{
    BOOST_ASSERT(channel_id != mixer::channel_id{});

    auto mixer_channels = st.mixer_state.channels.lock();
    auto& mixer_channel = mixer_channels[channel_id];

    auto fx_chain = mixer_channel.fx_chain.lock();

    auto id = st.fx_unavailable_ladspa_plugins.emplace(unavail);
    auto const insert_pos = std::min(position, fx_chain->size());
    fx_chain->emplace(
            std::next(fx_chain->begin(), insert_pos),
            st.fx_modules.insert({
                    .fx_instance_id = id,
                    .name = box(std::string(name)),
                    .bus_type = mixer_channel.bus_type,
                    .parameters = {},
                    .streams = {},
            }));
}

template <class P>
static auto
remove_midi_assignement_for_parameter(state& st, parameter::id_t<P> id)
{
    st.midi_assignments.lock()->erase(midi_assignment_id{id});
}

template <class P>
static auto
remove_parameter(state& st, parameter::id_t<P> id)
{
    st.params.remove(id);

    if constexpr (boost::mp11::mp_contains<
                          midi_assignment_id,
                          parameter::id_t<P>>::value)
    {
        remove_midi_assignement_for_parameter(st, id);
    }
}

void
remove_fx_module(
        state& st,
        mixer::channel_id const fx_chain_id,
        fx::module_id const fx_mod_id)
{
    fx::module const& fx_mod = st.fx_modules[fx_mod_id];

    auto mixer_channels = st.mixer_state.channels.lock();
    auto& mixer_channel = mixer_channels[fx_chain_id];

    BOOST_ASSERT(algorithm::contains(*mixer_channel.fx_chain, fx_mod_id));
    remove_erase(mixer_channel.fx_chain, fx_mod_id);

    for (auto&& [key, fx_param_id] : *fx_mod.parameters)
    {
        std::visit([&st](auto&& id) { remove_parameter(st, id); }, fx_param_id);
    }

    if (auto id = std::get_if<ladspa::instance_id>(&fx_mod.fx_instance_id))
    {
        st.fx_ladspa_instances.erase(*id);
    }
    else if (
            auto id = std::get_if<fx::unavailable_ladspa_id>(
                    &fx_mod.fx_instance_id))
    {
        st.fx_unavailable_ladspa_plugins.erase(*id);
    }

    st.fx_modules.erase(fx_mod_id);
}

template <class ParameterFactory>
static auto
make_aux_send_parameter(ParameterFactory& ui_params_factory)
{
    using namespace std::string_literals;

    return ui_params_factory.make_parameter(parameter::float_descriptor{
            .name = box("Send"s),
            .default_value = 0.f,
            .min = 0.f,
            .max = 1.f,
            .value_to_string = &volume_to_string,
            .to_normalized = &to_normalized_send,
            .from_normalized = &from_normalized_send});
}

template <class ParameterFactory>
static auto
make_aux_send(
        mixer::aux_sends_t& aux_sends,
        mixer::io_address_t const& route,
        ParameterFactory& ui_params_factory)
{
    aux_sends.emplace(
            route,
            mixer::aux_send{
                    .enabled = false,
                    .volume = make_aux_send_parameter(ui_params_factory)});
}

template <class ParameterFactory>
static auto
make_aux_sends(
        mixer::channels_t const& channels,
        external_audio::device_ids_t const& output_devices,
        ParameterFactory& ui_params_factory)
{
    mixer::aux_sends_t result;
    for (auto const& [channel_id, channel] : channels)
    {
        if (channel.bus_type == audio::bus_type::stereo)
        {
            make_aux_send(result, channel_id, ui_params_factory);
        }
    }

    // output device must be stereo
    for (auto const device_id : output_devices)
    {
        make_aux_send(result, device_id, ui_params_factory);
    }

    return result;
}

static auto
remove_aux_send(
        state& st,
        mixer::aux_sends_t& aux_sends,
        mixer::io_address_t const& route)
{
    if (auto it = aux_sends.find(route); it != aux_sends.end())
    {
        remove_parameter(st, it->second.volume);
        aux_sends.erase(it);
    }
}

auto
add_external_audio_device(
        state& st,
        std::string const& name,
        io_direction const io_dir,
        audio::bus_type const bus_type,
        channel_index_pair const& channels) -> external_audio::device_id
{
    auto boxed_name = box(name);

    auto name_id = string_id::generate();
    st.strings.insert(name_id, boxed_name);

    auto id = st.external_audio_state.devices.insert({
            .name = name_id,
            .bus_type = io_dir == io_direction::input ? bus_type
                                                      : audio::bus_type::stereo,
            .channels = channels,
    });

    auto& devices_ids = io_dir == io_direction::input
                                ? st.external_audio_state.inputs
                                : st.external_audio_state.outputs;

    emplace_back(devices_ids, id);

    parameter_factory params_factory{st.params};

    auto mixer_channels = st.mixer_state.channels.lock();

    for (auto& [_, mixer_channel] : mixer_channels)
    {
        if (io_dir == io_direction::output)
        {
            auto aux_sends = mixer_channel.aux_sends.lock();

            make_aux_send(*mixer_channel.aux_sends.lock(), id, params_factory);
        }
    }

    return id;
}

auto
add_mixer_channel(state& st, std::string name, audio::bus_type bus_type)
        -> mixer::channel_id
{
    using namespace std::string_literals;

    auto name_id = string_id::generate();
    st.strings.insert(name_id, box{std::move(name)});

    parameter_factory params_factory{st.params};
    auto mixer_channels = st.mixer_state.channels.lock();
    auto channel_id = mixer_channels.insert({
            .name = name_id,
            .bus_type = bus_type,
            .in = {},
            .out = st.mixer_state.main,
            .aux_sends = box(make_aux_sends(
                    st.mixer_state.channels,
                    st.external_audio_state.outputs,
                    params_factory)),
            .volume = params_factory.make_parameter(float_parameter{
                    .name = box("Volume"s),
                    .default_value = 1.f,
                    .min = 0.f,
                    .max = math::from_dB(6.f),
                    .value_to_string = &volume_to_string,
                    .to_normalized = &to_normalized_volume,
                    .from_normalized = &from_normalized_volume}),
            .pan_balance = params_factory.make_parameter(float_parameter{
                    .name = box(bus_type_to(bus_type, "Pan"s, "Balance"s)),
                    .default_value = 0.f,
                    .min = -1.f,
                    .max = 1.f,
                    .bipolar = true,
                    .to_normalized = &parameter::to_normalized_linear,
                    .from_normalized = &parameter::from_normalized_linear}),
            .record = params_factory.make_parameter(bool_parameter{
                    .name = box("Record"s),
                    .default_value = false}),
            .mute = params_factory.make_parameter(bool_parameter{
                    .name = box("Mute"s),
                    .default_value = false}),
            .solo = params_factory.make_parameter(bool_parameter{
                    .name = box("Solo"s),
                    .default_value = false}),
            .out_stream = make_stream(st.streams, 2),
            .fx_chain = {},
    });
    emplace_back(st.mixer_state.inputs, channel_id);

    // add as aux_send to each channel
    for (auto& [id, channel] : mixer_channels)
    {
        if (id != channel_id)
        {
            make_aux_send(
                    *channel.aux_sends.lock(),
                    channel_id,
                    params_factory);
        }
    }

    st.gui_state.mixer_colors.insert(channel_id, material_color::pink);

    return channel_id;
}

void
remove_mixer_channel(state& st, mixer::channel_id const mixer_channel_id)
{
    BOOST_ASSERT(mixer_channel_id != st.mixer_state.main);

    mixer::channel const& mixer_channel =
            st.mixer_state.channels[mixer_channel_id];

    st.strings.erase(mixer_channel.name);

    remove_parameter(st, mixer_channel.volume);
    remove_parameter(st, mixer_channel.pan_balance);
    remove_parameter(st, mixer_channel.record);
    remove_parameter(st, mixer_channel.mute);
    remove_parameter(st, mixer_channel.solo);

    // remove own aux_sends
    for (auto& aux_send : mixer_channel.aux_sends.get())
    {
        remove_parameter(st, aux_send.second.volume);
    }

    auto mixer_channels = st.mixer_state.channels.lock();

    // remove itself as aux_send from other channels
    auto const addr = mixer::io_address_t{mixer_channel_id};
    for (auto& [id, channel] : mixer_channels)
    {
        remove_aux_send(st, *channel.aux_sends.lock(), addr);
    }

    for (auto fx_mod_id : std::views::reverse(*mixer_channel.fx_chain))
    {
        remove_fx_module(st, mixer_channel_id, fx_mod_id);
    }

    if (st.gui_state.focused_fx_chain_id == mixer_channel_id)
    {
        st.gui_state.focused_fx_chain_id = {};
        st.gui_state.focused_fx_mod_id = {};
    }

    set_if(st.gui_state.fx_browser_fx_chain_id,
           equal_to(mixer_channel_id),
           mixer::channel_id{});

    BOOST_ASSERT(algorithm::contains(*st.mixer_state.inputs, mixer_channel_id));
    remove_erase(st.mixer_state.inputs, mixer_channel_id);

    st.streams.erase(mixer_channel.out_stream);

    mixer_channels.erase(mixer_channel_id);

    auto const equal_to_mixer_channel = equal_to(addr);
    for (auto& [_, channel] : mixer_channels)
    {
        set_if(channel.in,
               equal_to_mixer_channel,
               channel.bus_type == audio::bus_type::stereo
                       ? mixer::io_address_t{invalid_t{}}
                       : mixer::io_address_t{default_t{}});
        set_if(channel.out, equal_to_mixer_channel, default_t{});
        set_if(channel.aux, equal_to_mixer_channel, default_t{});
    }

    st.gui_state.mixer_colors.erase(mixer_channel_id);
}

void
remove_external_audio_device(
        state& st,
        external_audio::device_id const device_id)
{
    auto const name_id = st.external_audio_state.devices[device_id].name;
    auto const name = st.strings[name_id];

    st.strings.erase(name_id);

    auto mixer_channels = st.mixer_state.channels.lock();
    {
        auto const equal_to_device = equal_to(mixer::io_address_t(device_id));

        for (auto& [_, mixer_channel] : mixer_channels)
        {
            set_if(mixer_channel.in,
                   equal_to_device,
                   mixer_channel.bus_type == audio::bus_type::stereo
                           ? mixer::io_address_t{invalid_t{}}
                           : mixer::io_address_t{default_t{}});
            set_if(mixer_channel.out, equal_to_device, default_t{});
            set_if(mixer_channel.aux, equal_to_device, default_t{});
        }
    }

    st.external_audio_state.devices.erase(device_id);

    if (algorithm::contains(*st.external_audio_state.inputs, device_id))
    {
        remove_erase(st.external_audio_state.inputs, device_id);
    }
    else
    {
        BOOST_ASSERT(algorithm::contains(
                *st.external_audio_state.outputs,
                device_id));
        remove_erase(st.external_audio_state.outputs, device_id);

        auto const addr = mixer::io_address_t{device_id};
        for (auto& [_, mixer_channel] : mixer_channels)
        {
            remove_aux_send(st, *mixer_channel.aux_sends.lock(), addr);
        }
    }
}

void
update_midi_assignments(state& st, midi_assignments_map const& assignments)
{
    update_midi_assignments(*st.midi_assignments.lock(), assignments);
}

} // namespace piejam::runtime
