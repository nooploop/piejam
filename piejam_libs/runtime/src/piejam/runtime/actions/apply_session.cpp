// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/apply_session.h>

#include <piejam/functional/operators.h>
#include <piejam/range/indices.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/persistence/session.h>
#include <piejam/runtime/state.h>
#include <piejam/set_if.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::runtime::actions
{

namespace
{

template <io_direction D>
auto
apply_external_audio_device_configs(
        state& st,
        std::vector<persistence::session::external_audio_device_config> const&
                configs,
        std::size_t const num_ch)
{
    BOOST_ASSERT_MSG(
            D == io_direction::input ? st.external_audio_state.inputs->empty()
                                     : st.external_audio_state.outputs->empty(),
            "configs should be cleared before applying");
    auto const gte_num_ch = greater_equal<>(num_ch);
    for (auto const& config : configs)
    {
        auto chs = config.channels;
        set_if(chs.left, gte_num_ch, npos);
        set_if(chs.right, gte_num_ch, npos);

        auto const type = D == io_direction::output ? audio::bus_type::stereo
                                                    : config.bus_type;

        runtime::add_external_audio_device(st, config.name, D, type, chs);
    }
}

void
apply_mixer_fx_chain(
        state& st,
        mixer::channel_id const channel_id,
        persistence::session::fx_chain_t const& fx_chain_data)
{
    for (auto const& fx_plug : fx_chain_data)
    {
        std::visit(
                boost::hof::match(
                        [&](persistence::session::internal_fx const& fx) {
                            runtime::insert_internal_fx_module(
                                    st,
                                    channel_id,
                                    npos,
                                    fx.type,
                                    fx.preset,
                                    fx.midi);
                        },
                        [&](persistence::session::ladspa_plugin const&
                                    ladspa_plug) {
                            runtime::insert_missing_ladspa_fx_module(
                                    st,
                                    channel_id,
                                    npos,
                                    fx::unavailable_ladspa{
                                            .plugin_id = ladspa_plug.id,
                                            .parameter_values =
                                                    ladspa_plug.preset,
                                            .midi_assignments =
                                                    ladspa_plug.midi},
                                    ladspa_plug.name);
                        },
                        [](auto const&) { BOOST_ASSERT(false); }),
                fx_plug.as_variant());
    }
}

void
apply_mixer_midi(
        midi_assignments_map& assignments,
        mixer::channel const& mixer_channel,
        persistence::session::mixer_midi const& mixer_midi)
{
    if (mixer_midi.volume)
    {
        assignments.emplace(mixer_channel.volume, *mixer_midi.volume);
    }

    if (mixer_midi.pan)
    {
        assignments.emplace(mixer_channel.pan_balance, *mixer_midi.pan);
    }

    if (mixer_midi.mute)
    {
        assignments.emplace(mixer_channel.mute, *mixer_midi.mute);
    }
}

void
apply_mixer_parameters(
        parameters_map& params,
        mixer::channel const& mixer_channel,
        persistence::session::mixer_parameters const& mixer_params)
{
    params[mixer_channel.volume].value.set(mixer_params.volume);
    params[mixer_channel.pan_balance].value.set(mixer_params.pan);
    params[mixer_channel.mute].value.set(mixer_params.mute);
}

auto
find_mixer_channel_route(
        mixer::state const& mixer_state,
        std::size_t const& index)
{
    if (index == 0)
    {
        return mixer::io_address_t{mixer_state.main};
    }
    else if ((index - 1) < mixer_state.inputs->size())
    {
        return mixer::io_address_t{(*mixer_state.inputs)[index - 1]};
    }
    else
    {
        return mixer::io_address_t{};
    }
}

auto
find_external_audio_device_route(
        external_audio::device_ids_t const& devices,
        std::size_t const index,
        std::string const& name)
{
    return index < devices.size()
                   ? mixer::io_address_t{devices[index]}
                   : mixer::io_address_t{
                             mixer::missing_device_address{box(name)}};
}

void
apply_mixer_io(
        parameters_map& params,
        external_audio::state& external_audio_state,
        mixer::state& mixer_state,
        mixer::channel_id const& channel_id,
        persistence::session::mixer_io const& in,
        persistence::session::mixer_io const& out,
        std::vector<persistence::session::mixer_aux_send> const& aux_sends_data)
{
    auto find_route = [&](auto const& mixer_io, io_direction io_dir) {
        switch (mixer_io.type)
        {
            case persistence::session::mixer_io_type::device:
                return find_external_audio_device_route(
                        io_dir == io_direction::input
                                ? external_audio_state.inputs
                                : external_audio_state.outputs,
                        mixer_io.index,
                        mixer_io.name);

            case persistence::session::mixer_io_type::channel:
                return find_mixer_channel_route(mixer_state, mixer_io.index);

            default:
                return mixer::io_address_t();
        }
    };

    [&](mixer::channel& channel) {
        channel.in = find_route(in, io_direction::input);
        channel.out = find_route(out, io_direction::output);

        auto aux_sends = channel.aux_sends.lock();

        for (auto const& aux_send_data : aux_sends_data)
        {
            if (auto route =
                        find_route(aux_send_data.route, io_direction::output);
                !std::holds_alternative<default_t>(route))
            {
                if (auto aux_send_it = aux_sends->find(route);
                    aux_send_it != aux_sends->end())
                {
                    params[aux_send_it->second.volume].value.set(
                            aux_send_data.volume);

                    aux_send_it->second.enabled = aux_send_data.enabled;
                }
            }
        }
    }(mixer_state.channels.lock()[channel_id]);
}

} // namespace

void
apply_session::reduce(state& st) const
{
    apply_external_audio_device_configs<io_direction::input>(
            st,
            session->external_audio_input_devices,
            st.selected_io_sound_card.in.hw_params->num_channels);

    apply_external_audio_device_configs<io_direction::output>(
            st,
            session->external_audio_output_devices,
            st.selected_io_sound_card.out.hw_params->num_channels);

    BOOST_ASSERT(st.mixer_state.inputs->empty());
    BOOST_ASSERT(st.mixer_state.channels.size() == 1);
    BOOST_ASSERT(st.mixer_state.channels.find(st.mixer_state.main));

    midi_assignments_map mixer_midi_assignments;
    for (auto const& channel_data : session->mixer_channels)
    {
        auto added_channel_id = runtime::add_mixer_channel(
                st,
                channel_data.name,
                channel_data.bus_type);
        auto& added_channel = st.mixer_state.channels[added_channel_id];
        apply_mixer_midi(
                mixer_midi_assignments,
                added_channel,
                channel_data.midi);
        apply_mixer_parameters(
                st.params,
                added_channel,
                channel_data.parameter);
        apply_mixer_fx_chain(st, added_channel_id, channel_data.fx_chain);
    }

    [&](mixer::channel& main_mixer_channel) {
        main_mixer_channel.name = session->main_mixer_channel.name;
        apply_mixer_midi(
                mixer_midi_assignments,
                main_mixer_channel,
                session->main_mixer_channel.midi);
        apply_mixer_parameters(
                st.params,
                main_mixer_channel,
                session->main_mixer_channel.parameter);
        apply_mixer_fx_chain(
                st,
                st.mixer_state.main,
                session->main_mixer_channel.fx_chain);
    }(st.mixer_state.channels.lock()[st.mixer_state.main]);

    runtime::update_midi_assignments(st, mixer_midi_assignments);

    // mixer I/O can only be applied after all channels are created
    for (std::size_t channel_index : range::indices(session->mixer_channels))
    {
        auto channel_id = (*st.mixer_state.inputs)[channel_index];
        auto const& channel_data = session->mixer_channels[channel_index];

        apply_mixer_io(
                st.params,
                st.external_audio_state,
                st.mixer_state,
                channel_id,
                channel_data.in,
                channel_data.out,
                channel_data.aux_sends);
    }

    apply_mixer_io(
            st.params,
            st.external_audio_state,
            st.mixer_state,
            st.mixer_state.main,
            session->main_mixer_channel.in,
            session->main_mixer_channel.out,
            session->main_mixer_channel.aux_sends);
}

} // namespace piejam::runtime::actions
