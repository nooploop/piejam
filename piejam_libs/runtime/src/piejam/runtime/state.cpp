// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/state.h>

#include <piejam/algorithm/contains.h>
#include <piejam/audio/types.h>
#include <piejam/functional/operators.h>
#include <piejam/indexed_access.h>
#include <piejam/ladspa/port_descriptor.h>
#include <piejam/runtime/internal_fx_module_factory.h>
#include <piejam/runtime/ladspa_fx/ladspa_fx_module.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/parameter_value_to_string.h>
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

struct volume_low_dB_interval
{
    static constexpr float min = -60.f;
    static constexpr float max = -12.f;
};

struct volume_high_dB_interval
{
    static constexpr float min = -12.f;
    static constexpr float max = 12.f;
};

constexpr auto
to_normalized_volume(float_parameter const& p, float const value)
{
    if (value == 0.f)
    {
        return 0.f;
    }
    else if (0.f < value && value < 0.0625f)
    {
        return parameter::to_normalized_dB<volume_low_dB_interval>(p, value) /
               4.f;
    }
    else
    {
        return (parameter::to_normalized_dB<volume_high_dB_interval>(p, value) *
                0.75f) +
               0.25f;
    }
}

constexpr auto
from_normalized_volume(float_parameter const& p, float const norm_value)
        -> float
{
    if (norm_value == 0.f)
    {
        return 0.f;
    }
    else if (0.f < norm_value && norm_value < 0.25f)
    {
        return parameter::from_normalized_dB<volume_low_dB_interval>(
                p,
                4.f * norm_value);
    }
    else
    {
        return parameter::from_normalized_dB<volume_high_dB_interval>(
                p,
                (norm_value - 0.25f) / 0.75f);
    }
}

struct send_low_dB_interval
{
    static constexpr float min = -60.f;
    static constexpr float max = -24.f;
};

struct send_high_dB_interval
{
    static constexpr float min = -24.f;
    static constexpr float max = 0.f;
};

constexpr auto
to_normalized_send(float_parameter const& p, float const value)
{
    if (value == 0.f)
    {
        return 0.f;
    }
    else if (0.f < value && value < 0.0625f)
    {
        return parameter::to_normalized_dB<send_low_dB_interval>(p, value) /
               4.f;
    }
    else
    {
        return (parameter::to_normalized_dB<send_high_dB_interval>(p, value) *
                0.75f) +
               0.25f;
    }
}

constexpr auto
from_normalized_send(float_parameter const& p, float const norm_value) -> float
{
    if (norm_value == 0.f)
    {
        return 0.f;
    }
    else if (0.f < norm_value && norm_value < 0.25f)
    {
        return parameter::from_normalized_dB<send_low_dB_interval>(
                p,
                4.f * norm_value);
    }
    else
    {
        return parameter::from_normalized_dB<send_high_dB_interval>(
                p,
                (norm_value - 0.25f) / 0.75f);
    }
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
    st.mixer_state.channels.update(
            st.mixer_state.main,
            [](mixer::channel& mixer_channel) {
                mixer_channel.out = piejam::default_t{};
            });
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
    return fx_modules.add(std::move(fx_mod));
}

static void
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

static void
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
                    .ui_params = st.ui_params,
                    .streams = st.streams,
            }));

    fx_chain.emplace(std::next(fx_chain.begin(), insert_pos), fx_mod_id);

    auto const& fx_mod = st.fx_modules[fx_chain[insert_pos]];

    apply_parameter_values(initial_values, fx_mod, st.params);
    apply_fx_midi_assignments(
            midi_assigns,
            fx_mod,
            *st.midi_assignments.lock());

    st.mixer_state.channels.update(
            mixer_channel_id,
            [&](mixer::channel& mixer_channel) {
                mixer_channel.fx_chain = std::move(fx_chain);
            });

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

    auto fx_mod_id = st.fx_modules.add(ladspa_fx::make_module(
            instance_id,
            plugin_desc.name,
            bus_type,
            control_inputs,
            st.params,
            st.ui_params));

    fx_chain.emplace(std::next(fx_chain.begin(), insert_pos), fx_mod_id);

    auto const& fx_mod = st.fx_modules[fx_chain[insert_pos]];
    apply_parameter_values(initial_values, fx_mod, st.params);
    apply_fx_midi_assignments(
            midi_assigns,
            fx_mod,
            *st.midi_assignments.lock());

    st.mixer_state.channels.update(
            mixer_channel_id,
            [&](mixer::channel& mixer_channel) {
                mixer_channel.fx_chain = std::move(fx_chain);
            });

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

    st.mixer_state.channels.update(
            channel_id,
            [&](mixer::channel& mixer_channel) {
                auto fx_chain = mixer_channel.fx_chain.lock();

                auto id = st.fx_unavailable_ladspa_plugins.add(unavail);
                auto const insert_pos = std::min(position, fx_chain->size());
                fx_chain->emplace(
                        std::next(fx_chain->begin(), insert_pos),
                        st.fx_modules.add(fx::module{
                                .fx_instance_id = id,
                                .name = box(std::string(name)),
                                .parameters = {},
                                .streams = {}}));
            });
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
    st.ui_params.remove(id);

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

    st.mixer_state.channels.update(
            fx_chain_id,
            [fx_mod_id](mixer::channel& mixer_channel) {
                BOOST_ASSERT(algorithm::contains(
                        *mixer_channel.fx_chain,
                        fx_mod_id));
                remove_erase(mixer_channel.fx_chain, fx_mod_id);
            });

    for (auto&& [key, fx_param_id] : *fx_mod.parameters)
    {
        std::visit([&st](auto&& id) { remove_parameter(st, id); }, fx_param_id);
    }

    st.fx_modules.remove(fx_mod_id);

    if (auto id = std::get_if<ladspa::instance_id>(&fx_mod.fx_instance_id))
    {
        st.fx_ladspa_instances.erase(*id);
    }
    else if (
            auto id = std::get_if<fx::unavailable_ladspa_id>(
                    &fx_mod.fx_instance_id))
    {
        st.fx_unavailable_ladspa_plugins.remove(*id);
    }
}

template <class ParameterFactory>
static auto
make_aux_send_parameter(ParameterFactory& ui_params_factory)
{
    using namespace std::string_literals;

    return ui_params_factory.make_parameter(
            parameter::float_{
                    .default_value = 0.f,
                    .min = 0.f,
                    .max = 1.f,
                    .to_normalized = &to_normalized_send,
                    .from_normalized = &from_normalized_send},
            {.name = box("Send"s), .value_to_string = &volume_to_string});
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

    auto id = st.external_audio_state.devices.add(external_audio::device{
            .name = boxed_name,
            .bus_type = io_dir == io_direction::input ? bus_type
                                                      : audio::bus_type::stereo,
            .channels = channels});

    auto& devices_ids = io_dir == io_direction::input
                                ? st.external_audio_state.inputs
                                : st.external_audio_state.outputs;

    emplace_back(devices_ids, id);

    parameter_factory ui_params_factory{st.params, st.ui_params};

    st.mixer_state.channels.update(
            [&,
             io_dir,
             equal_to_device_name = equal_to<>(mixer::io_address_t(
                     mixer::missing_device_address{boxed_name})),
             id](mixer::channel_id, mixer::channel& mixer_channel) {
                if (io_dir == io_direction::input)
                {
                    set_if(mixer_channel.in, equal_to_device_name, id);
                }
                else
                {
                    set_if(mixer_channel.out, equal_to_device_name, id);
                }

                if (io_dir == io_direction::output)
                {
                    auto aux_sends = mixer_channel.aux_sends.lock();

                    make_aux_send(
                            *mixer_channel.aux_sends.lock(),
                            id,
                            ui_params_factory);
                }
            });

    return id;
}

auto
add_mixer_channel(state& st, std::string name, audio::bus_type bus_type)
        -> mixer::channel_id
{
    using namespace std::string_literals;

    parameter_factory ui_params_factory{st.params, st.ui_params};
    auto channel_id = st.mixer_state.channels.add(mixer::channel{
            .name = box(std::move(name)),
            .bus_type = bus_type,
            .in = {},
            .out = st.mixer_state.main,
            .aux_sends = box(make_aux_sends(
                    st.mixer_state.channels,
                    st.external_audio_state.outputs,
                    ui_params_factory)),
            .volume = ui_params_factory.make_parameter(
                    parameter::float_{
                            .default_value = 1.f,
                            .min = 0.f,
                            .max = 4.f,
                            .to_normalized = &to_normalized_volume,
                            .from_normalized = &from_normalized_volume},
                    {.name = box("Volume"s),
                     .value_to_string = &volume_to_string}),
            .pan_balance = ui_params_factory.make_parameter(
                    parameter::float_{
                            .default_value = 0.f,
                            .min = -1.f,
                            .max = 1.f,
                            .to_normalized = &parameter::to_normalized_linear,
                            .from_normalized =
                                    &parameter::from_normalized_linear},
                    {.name = box(bus_type_to(bus_type, "Pan"s, "Balance"s)),
                     .value_to_string = &float_parameter_value_to_string}),
            .record = ui_params_factory.make_parameter(
                    parameter::bool_{.default_value = false},
                    {.name = box("Record"s),
                     .value_to_string = &bool_parameter_value_to_string}),
            .mute = ui_params_factory.make_parameter(
                    parameter::bool_{.default_value = false},
                    {.name = box("Mute"s),
                     .value_to_string = &bool_parameter_value_to_string}),
            .solo = ui_params_factory.make_parameter(
                    parameter::bool_{.default_value = false},
                    {.name = box("Solo"s),
                     .value_to_string = &bool_parameter_value_to_string}),
            .peak_level = parameter_factory{st.params}.make_parameter(
                    parameter::stereo_level{}),
            .rms_level = parameter_factory{st.params}.make_parameter(
                    parameter::stereo_level{}),
            .fx_chain = {}});
    emplace_back(st.mixer_state.inputs, channel_id);

    // add as aux_send to each channel
    st.mixer_state.channels.update(
            [&](mixer::channel_id id, mixer::channel& channel) {
                if (id != channel_id)
                {
                    make_aux_send(
                            *channel.aux_sends.lock(),
                            channel_id,
                            ui_params_factory);
                }
            });

    return channel_id;
}

void
remove_mixer_channel(state& st, mixer::channel_id const mixer_channel_id)
{
    BOOST_ASSERT(mixer_channel_id != st.mixer_state.main);

    mixer::channel const& mixer_channel =
            st.mixer_state.channels[mixer_channel_id];

    remove_parameter(st, mixer_channel.volume);
    remove_parameter(st, mixer_channel.pan_balance);
    remove_parameter(st, mixer_channel.record);
    remove_parameter(st, mixer_channel.mute);
    remove_parameter(st, mixer_channel.solo);
    remove_parameter(st, mixer_channel.peak_level);
    remove_parameter(st, mixer_channel.rms_level);

    // remove own aux_sends
    for (auto& aux_send : mixer_channel.aux_sends.get())
    {
        remove_parameter(st, aux_send.second.volume);
    }

    // remove itself as aux_send from other channels
    st.mixer_state.channels.update(
            [&, addr = mixer::io_address_t{mixer_channel_id}](
                    mixer::channel_id,
                    mixer::channel& channel) {
                remove_aux_send(st, *channel.aux_sends.lock(), addr);
            });

    BOOST_ASSERT_MSG(
            mixer_channel.fx_chain->empty(),
            "fx_chain should be emptied before");

    BOOST_ASSERT(algorithm::contains(*st.mixer_state.inputs, mixer_channel_id));
    remove_erase(st.mixer_state.inputs, mixer_channel_id);

    st.mixer_state.channels.remove(mixer_channel_id);

    st.mixer_state.channels.update(
            [equal_to_mixer_channel =
                     equal_to<>(mixer::io_address_t(mixer_channel_id)),
             empty_addr = mixer::io_address_t{}](
                    mixer::channel_id,
                    mixer::channel& mixer_channel) {
                set_if(mixer_channel.in, equal_to_mixer_channel, empty_addr);
                set_if(mixer_channel.out, equal_to_mixer_channel, empty_addr);
                set_if(mixer_channel.aux, equal_to_mixer_channel, empty_addr);
            });
}

void
remove_external_audio_device(
        state& st,
        external_audio::device_id const device_id)
{
    auto const name = st.external_audio_state.devices[device_id].name;

    st.mixer_state.channels.update(
            [equal_to_device = equal_to<>(mixer::io_address_t(device_id)),
             missing_device_name = mixer::missing_device_address(
                     name)](mixer::channel_id, mixer::channel& mixer_channel) {
                set_if(mixer_channel.in, equal_to_device, missing_device_name);
                set_if(mixer_channel.out, equal_to_device, missing_device_name);
                set_if(mixer_channel.aux, equal_to_device, missing_device_name);
            });

    st.external_audio_state.devices.remove(device_id);

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

        st.mixer_state.channels.update(
                [&, route = mixer::io_address_t{device_id}](
                        mixer::channel_id,
                        mixer::channel& mixer_channel) {
                    remove_aux_send(st, *mixer_channel.aux_sends.lock(), route);
                });
    }
}

void
update_midi_assignments(state& st, midi_assignments_map const& assignments)
{
    update_midi_assignments(*st.midi_assignments.lock(), assignments);
}

} // namespace piejam::runtime
