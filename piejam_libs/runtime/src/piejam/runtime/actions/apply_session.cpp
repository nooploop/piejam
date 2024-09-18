// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/apply_session.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>
#include <piejam/runtime/actions/control_midi_assignment.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/persistence/session.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <boost/assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include <functional>

namespace piejam::runtime::actions
{

namespace
{

struct make_add_fx_module_action
{
    mixer::channel_id fx_chain_id;

    auto operator()(persistence::session::internal_fx const& fx) const
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::insert_internal_fx_module>();
        action->fx_chain_id = fx_chain_id;
        action->position = npos;
        action->type = fx.type;
        action->initial_values = fx.preset;
        action->midi_assignments = fx.midi;
        return action;
    }

    auto operator()(persistence::session::ladspa_plugin const& ladspa_plug)
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::load_ladspa_fx_plugin>();
        action->fx_chain_id = fx_chain_id;
        action->position = npos;
        action->plugin_id = ladspa_plug.id;
        action->name = ladspa_plug.name;
        action->initial_values = ladspa_plug.preset;
        action->midi_assignments = ladspa_plug.midi;
        return action;
    }

    auto operator()(std::monostate const&) -> std::unique_ptr<action>
    {
        BOOST_ASSERT(false);
        return nullptr;
    }
};

void
make_add_fx_module_actions(
        batch_action& batch,
        mixer::channels_t const& channels,
        mixer::channel_ids_t const& channel_ids,
        std::vector<persistence::session::mixer_channel> const& mb_data)
{
    boost::ignore_unused(channels);
    BOOST_ASSERT(channel_ids.size() == mb_data.size());
    for (std::size_t i : range::indices(channel_ids))
    {
        mixer::channel_id const fx_chain_id = channel_ids[i];
        BOOST_ASSERT(channels[fx_chain_id].fx_chain->empty());
        for (auto const& fx_plug : mb_data[i].fx_chain)
        {
            batch.push_back(std::visit(
                    make_add_fx_module_action{fx_chain_id},
                    fx_plug.as_variant()));
        }
    }
}

void
apply_mixer_midi(
        actions::update_midi_assignments& action,
        mixer::channels_t const& channels,
        mixer::channel_id const channel_id,
        persistence::session::mixer_midi const& mixer_midi)
{
    mixer::channel const& mixer_channel = channels[channel_id];

    if (mixer_midi.volume)
    {
        action.assignments.emplace(mixer_channel.volume, *mixer_midi.volume);
    }

    if (mixer_midi.pan)
    {
        action.assignments.emplace(mixer_channel.pan_balance, *mixer_midi.pan);
    }

    if (mixer_midi.mute)
    {
        action.assignments.emplace(mixer_channel.mute, *mixer_midi.mute);
    }
}

void
apply_mixer_midi(
        actions::update_midi_assignments& action,
        mixer::channels_t const& channels,
        mixer::channel_ids_t const& channel_ids,
        std::vector<persistence::session::mixer_channel> const& mb_data)
{
    BOOST_ASSERT(mb_data.size() == channel_ids.size());

    for (std::size_t const i : range::indices(channel_ids))
    {
        apply_mixer_midi(action, channels, channel_ids[i], mb_data[i].midi);
    }
}

void
apply_mixer_parameters(
        batch_action& batch,
        mixer::channel const& mixer_channel,
        persistence::session::mixer_parameters const& mixer_params)
{
    batch.emplace_back<actions::set_float_parameter>(
            mixer_channel.volume,
            mixer_params.volume);
    batch.emplace_back<actions::set_float_parameter>(
            mixer_channel.pan_balance,
            mixer_params.pan);
    batch.emplace_back<actions::set_bool_parameter>(
            mixer_channel.mute,
            mixer_params.mute);
}

void
apply_mixer_parameters(
        batch_action& batch,
        mixer::channels_t const& channels,
        mixer::channel_ids_t const& channel_ids,
        std::vector<persistence::session::mixer_channel> const& mb_data)
{
    BOOST_ASSERT(mb_data.size() == channel_ids.size());
    for (std::size_t const i : range::indices(channel_ids))
    {
        mixer::channel const& mixer_channel = channels[channel_ids[i]];
        apply_mixer_parameters(batch, mixer_channel, mb_data[i].parameter);
    }
}

auto
find_mixer_channel_route(
        mixer::channels_t const& channels,
        std::string const& name)
{
    if (auto it = std::ranges::find_if(
                channels,
                [&name](auto const& p) { return p.second.name == name; });
        it != channels.end())
    {
        return mixer::io_address_t(it->first);
    }
    else
    {
        return mixer::io_address_t{};
    }
}

auto
find_external_audio_device_route(
        external_audio::devices_t const& devices,
        std::string const& name)
{
    if (auto it = std::ranges::find_if(
                devices,
                [&name](auto const& p) { return p.second.name == name; });
        it != devices.end())
    {
        return mixer::io_address_t(it->first);
    }
    else
    {
        return mixer::io_address_t(mixer::missing_device_address{box_(name)});
    }
}

void
apply_mixer_io(
        batch_action& batch,
        external_audio::devices_t const& devices,
        mixer::channels_t const& channels,
        mixer::channel_id const& channel_id,
        persistence::session::mixer_io const& in,
        persistence::session::mixer_io const& out,
        std::vector<persistence::session::mixer_aux_send> const& aux_sends)
{
    auto find_route = [&](auto const& mixer_io) {
        switch (mixer_io.type)
        {
            case persistence::session::mixer_io_type::device:
                return find_external_audio_device_route(devices, mixer_io.name);

            case persistence::session::mixer_io_type::channel:
                return find_mixer_channel_route(channels, mixer_io.name);

            default:
                return mixer::io_address_t();
        }
    };

    {
        auto action = std::make_unique<set_mixer_channel_route>();
        action->channel_id = channel_id;
        action->io_socket = mixer::io_socket::in;
        action->route = find_route(in);
        batch.push_back(std::move(action));
    }

    {
        auto action = std::make_unique<set_mixer_channel_route>();
        action->channel_id = channel_id;
        action->io_socket = mixer::io_socket::out;
        action->route = find_route(out);
        batch.push_back(std::move(action));
    }

    if (auto const* const channel = channels.find(channel_id); channel)
    {
        for (auto const& aux_send_data : aux_sends)
        {
            if (auto route = find_route(aux_send_data.route);
                !std::holds_alternative<default_t>(route))
            {
                auto const aux_send_it = channel->aux_sends->find(route);

                batch.emplace_back<actions::set_float_parameter>(
                        aux_send_it->second.volume,
                        aux_send_data.volume);

                if (aux_send_data.enabled)
                {
                    auto action = std::make_unique<
                            actions::enable_mixer_channel_aux_route>();
                    action->channel_id = channel_id;
                    action->route = route;
                    action->enabled = true;
                    batch.push_back(std::move(action));
                }
            }
        }
    }
}

void
apply_mixer_io(
        batch_action& batch,
        external_audio::devices_t const& device_buses,
        mixer::channels_t const& channels,
        mixer::channel_ids_t const& channel_ids,
        std::vector<persistence::session::mixer_channel> const& mb_data)
{
    BOOST_ASSERT(mb_data.size() == channel_ids.size());
    for (std::size_t const i : range::indices(channel_ids))
    {
        apply_mixer_io(
                batch,
                device_buses,
                channels,
                channel_ids[i],
                mb_data[i].in,
                mb_data[i].out,
                mb_data[i].aux_sends);
    }
}

template <class GetState>
auto
configure_mixer_channels(
        persistence::session const& session,
        GetState&& get_state) -> batch_action
{
    batch_action action;

    state const& st = get_state();

    // fx modules
    {
        make_add_fx_module_actions(
                action,
                st.mixer_state.channels,
                st.mixer_state.inputs,
                session.mixer_channels);

        BOOST_ASSERT(
                st.mixer_state.channels[st.mixer_state.main].fx_chain->empty());
        for (auto const& fx_plug : session.main_mixer_channel.fx_chain)
        {
            action.push_back(std::visit(
                    make_add_fx_module_action{st.mixer_state.main},
                    fx_plug.as_variant()));
        }
    }

    // midi
    {
        auto midi_action = std::make_unique<actions::update_midi_assignments>();

        apply_mixer_midi(
                *midi_action,
                st.mixer_state.channels,
                st.mixer_state.inputs,
                session.mixer_channels);

        apply_mixer_midi(
                *midi_action,
                st.mixer_state.channels,
                st.mixer_state.main,
                session.main_mixer_channel.midi);

        if (!midi_action->assignments.empty())
        {
            action.push_back(std::move(midi_action));
        }
    }

    // paramaeters
    apply_mixer_parameters(
            action,
            st.mixer_state.channels,
            st.mixer_state.inputs,
            session.mixer_channels);
    apply_mixer_parameters(
            action,
            st.mixer_state.channels[st.mixer_state.main],
            session.main_mixer_channel.parameter);

    // io
    apply_mixer_io(
            action,
            st.external_audio_state.devices,
            st.mixer_state.channels,
            st.mixer_state.inputs,
            session.mixer_channels);
    apply_mixer_io(
            action,
            st.external_audio_state.devices,
            st.mixer_state.channels,
            st.mixer_state.main,
            session.main_mixer_channel.in,
            session.main_mixer_channel.out,
            session.main_mixer_channel.aux_sends);

    return action;
}

auto
apply_session_data(persistence::session session)
        -> std::unique_ptr<thunk_action>
{
    return std::make_unique<thunk_action>(
            [s = std::move(session)](auto&& get_state, auto&& dispatch) {
                std::invoke(
                        std::forward<decltype(dispatch)>(dispatch),
                        configure_mixer_channels(
                                s,
                                std::forward<decltype(get_state)>(get_state)));
            });
}

auto
make_mixer_channels(persistence::session session) -> thunk_action
{
    return [session = std::move(session)](auto&& get_state, auto&& dispatch) {
        state const& st = get_state();
        BOOST_ASSERT(st.mixer_state.inputs->empty());
        BOOST_ASSERT(st.mixer_state.channels.size() == 1);

        batch_action action;

        for (auto const& mb_data : session.mixer_channels)
        {
            action.emplace_back<actions::add_mixer_channel>(
                    mb_data.name,
                    mb_data.bus_type);
        }

        auto set_main_name_action =
                std::make_unique<actions::set_mixer_channel_name>();
        set_main_name_action->channel_id = st.mixer_state.main;
        set_main_name_action->name = session.main_mixer_channel.name;
        action.push_back(std::move(set_main_name_action));

        action.push_back(apply_session_data(std::move(session)));

        dispatch(action);
    };
}

} // namespace

auto
apply_session(persistence::session session) -> thunk_action
{
    return make_mixer_channels(std::move(session));
}

} // namespace piejam::runtime::actions
