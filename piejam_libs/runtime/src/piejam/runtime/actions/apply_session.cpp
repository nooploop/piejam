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

#include <boost/core/ignore_unused.hpp>

#include <functional>

namespace piejam::runtime::actions
{

namespace
{

struct make_add_fx_module_action
{
    mixer::bus_id fx_chain_bus;

    auto operator()(persistence::session::internal_fx const& fx) const
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::insert_internal_fx_module>();
        action->fx_chain_bus = fx_chain_bus;
        action->position = npos;
        action->type = fx.type;
        action->initial_values = fx.preset;
        action->midi_assigns = fx.midi;
        return action;
    }

    auto operator()(persistence::session::ladspa_plugin const& ladspa_plug)
            -> std::unique_ptr<action>
    {
        auto action = std::make_unique<actions::load_ladspa_fx_plugin>();
        action->fx_chain_bus = fx_chain_bus;
        action->position = npos;
        action->plugin_id = ladspa_plug.id;
        action->name = ladspa_plug.name;
        action->initial_values = ladspa_plug.preset;
        action->midi_assigns = ladspa_plug.midi;
        return action;
    }
};

void
make_add_fx_module_actions(
        batch_action& batch,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& bus_ids,
        std::vector<persistence::session::mixer_bus> const& mb_data)
{
    boost::ignore_unused(buses);
    BOOST_ASSERT(bus_ids.size() == mb_data.size());
    for (std::size_t i : range::indices(bus_ids))
    {
        mixer::bus_id const fx_chain_bus = bus_ids[i];
        BOOST_ASSERT(buses[fx_chain_bus].fx_chain->empty());
        for (auto const& fx_plug : mb_data[i].fx_chain)
        {
            batch.push_back(std::visit(
                    make_add_fx_module_action{fx_chain_bus},
                    fx_plug.as_variant()));
        }
    }
}

void
apply_mixer_midi(
        actions::update_midi_assignments& action,
        mixer::buses_t const& buses,
        mixer::bus_id const bus_id,
        persistence::session::mixer_midi const& mixer_midi)
{
    mixer::bus const& bus = buses[bus_id];

    if (mixer_midi.volume)
        action.assignments.emplace(bus.volume, *mixer_midi.volume);

    if (mixer_midi.pan)
        action.assignments.emplace(bus.pan_balance, *mixer_midi.pan);

    if (mixer_midi.mute)
        action.assignments.emplace(bus.mute, *mixer_midi.mute);
}

void
apply_mixer_midi(
        actions::update_midi_assignments& action,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& bus_ids,
        std::vector<persistence::session::mixer_bus> const& mb_data)
{
    BOOST_ASSERT(mb_data.size() == bus_ids.size());

    for (std::size_t const i : range::indices(bus_ids))
        apply_mixer_midi(action, buses, bus_ids[i], mb_data[i].midi);
}

void
apply_mixer_parameters(
        batch_action& batch,
        mixer::bus const& bus,
        persistence::session::mixer_parameters const& mixer_params)
{
    batch.emplace_back<actions::set_float_parameter>(
            bus.volume,
            mixer_params.volume);
    batch.emplace_back<actions::set_float_parameter>(
            bus.pan_balance,
            mixer_params.pan);
    batch.emplace_back<actions::set_bool_parameter>(
            bus.mute,
            mixer_params.mute);
}

void
apply_mixer_parameters(
        batch_action& batch,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& bus_ids,
        std::vector<persistence::session::mixer_bus> const& mb_data)
{
    BOOST_ASSERT(mb_data.size() == bus_ids.size());
    for (std::size_t const i : range::indices(bus_ids))
    {
        mixer::bus const& bus = buses[bus_ids[i]];
        apply_mixer_parameters(batch, bus, mb_data[i].parameter);
    }
}

auto
find_mixer_bus_by_name(
        mixer::buses_t const& mixer_buses,
        std::string const& name)
{
    return std::ranges::find_if(mixer_buses, [&name](auto const& p) {
        return p.second.name == name;
    });
}

auto
find_device_bus_by_name(
        device_io::buses_t const& device_buses,
        std::string const& name)
{
    return std::ranges::find_if(device_buses, [&name](auto const& p) {
        return p.second.name == name;
    });
}

void
apply_mixer_io(
        batch_action& batch,
        device_io::buses_t const& device_buses,
        mixer::buses_t const& mixer_buses,
        mixer::bus_id const& bus_id,
        persistence::session::mixer_io const& in,
        persistence::session::mixer_io const& out)
{
    auto get_io_addr = [&](auto const& mixer_io) {
        switch (mixer_io.type)
        {
            case persistence::session::mixer_io_type::device:
                if (auto it = find_device_bus_by_name(
                            device_buses,
                            mixer_io.name);
                    it != device_buses.end())
                    return mixer::io_address_t(it->first);
                else
                    return mixer::io_address_t(mixer_io.name);

            case persistence::session::mixer_io_type::channel:
                if (auto it =
                            find_mixer_bus_by_name(mixer_buses, mixer_io.name);
                    it != mixer_buses.end())
                    return mixer::io_address_t(it->first);
                else
                    return mixer::io_address_t();

            default:
                return mixer::io_address_t();
        }
    };

    {
        auto action = std::make_unique<set_mixer_channel_input>();
        action->bus_id = bus_id;
        action->route = get_io_addr(in);
        batch.push_back(std::move(action));
    }

    {
        auto action = std::make_unique<set_mixer_channel_output>();
        action->bus_id = bus_id;
        action->route = get_io_addr(out);
        batch.push_back(std::move(action));
    }
}

void
apply_mixer_io(
        batch_action& batch,
        device_io::buses_t const& device_buses,
        mixer::buses_t const& buses,
        mixer::bus_list_t const& mixer_bus_ids,
        std::vector<persistence::session::mixer_bus> const& mb_data)
{
    BOOST_ASSERT(mb_data.size() == mixer_bus_ids.size());
    for (std::size_t const i : range::indices(mixer_bus_ids))
    {
        apply_mixer_io(
                batch,
                device_buses,
                buses,
                mixer_bus_ids[i],
                mb_data[i].in,
                mb_data[i].out);
    }
}

template <class GetState>
auto
configure_mixer_buses(persistence::session const& session, GetState&& get_state)
        -> batch_action
{
    batch_action action;

    state const& st = get_state();

    // fx modules
    {
        make_add_fx_module_actions(
                action,
                st.mixer_state.buses,
                st.mixer_state.inputs,
                session.mixer_channels);

        BOOST_ASSERT(
                st.mixer_state.buses[st.mixer_state.main].fx_chain->empty());
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
                st.mixer_state.buses,
                st.mixer_state.inputs,
                session.mixer_channels);

        apply_mixer_midi(
                *midi_action,
                st.mixer_state.buses,
                st.mixer_state.main,
                session.main_mixer_channel.midi);

        if (!midi_action->assignments.empty())
            action.push_back(std::move(midi_action));
    }

    // paramaeters
    apply_mixer_parameters(
            action,
            st.mixer_state.buses,
            st.mixer_state.inputs,
            session.mixer_channels);
    apply_mixer_parameters(
            action,
            st.mixer_state.buses[st.mixer_state.main],
            session.main_mixer_channel.parameter);

    // io
    apply_mixer_io(
            action,
            st.device_io_state.buses,
            st.mixer_state.buses,
            st.mixer_state.inputs,
            session.mixer_channels);
    apply_mixer_io(
            action,
            st.device_io_state.buses,
            st.mixer_state.buses,
            st.mixer_state.main,
            session.main_mixer_channel.in,
            session.main_mixer_channel.out);

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
                        configure_mixer_buses(
                                s,
                                std::forward<decltype(get_state)>(get_state)));
            });
}

auto
make_mixer_buses(persistence::session session) -> thunk_action
{
    return [session = std::move(session)](auto&& get_state, auto&& dispatch) {
        state const& st = get_state();
        BOOST_ASSERT(st.mixer_state.inputs->empty());
        BOOST_ASSERT(st.mixer_state.buses.size() == 1);

        batch_action action;

        for (auto const& mb_data : session.mixer_channels)
            action.emplace_back<actions::add_mixer_bus>(mb_data.name);

        auto set_main_name_action =
                std::make_unique<actions::set_mixer_bus_name>();
        set_main_name_action->bus_id = st.mixer_state.main;
        set_main_name_action->name = session.main_mixer_channel.name;
        action.push_back(std::move(set_main_name_action));

        action.push_back(apply_session_data(std::move(session)));

        dispatch(action);
    };
}

} // namespace

void
apply_session(persistence::session session, dispatch_f const& dispatch)
{
    dispatch(make_mixer_buses(std::move(session)));
}

} // namespace piejam::runtime::actions
