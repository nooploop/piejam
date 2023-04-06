// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/midi_control_middleware.h>

#include <piejam/algorithm/contains.h>
#include <piejam/algorithm/for_each_visit.h>
#include <piejam/midi/device_update.h>
#include <piejam/runtime/actions/activate_midi_device.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/refresh_midi_devices.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/actions/request_parameters_update.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/state.h>

#include <boost/assert.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace piejam::runtime
{

namespace
{

struct update_midi_devices final
    : ui::cloneable_action<update_midi_devices, reducible_action>
{
    std::vector<midi::device_update> updates;

    struct midi_device_update_handler
    {
        midi_devices_t& midi_devices;
        std::vector<midi::device_id_t>& midi_inputs;

        void operator()(midi::device_added const& op) const
        {
            midi_devices.emplace(
                    op.device_id,
                    midi_device_config{.name = op.name, .enabled = false});
            midi_inputs.emplace_back(op.device_id);
        }

        void operator()(midi::device_removed const& op) const
        {
            midi_devices.erase(op.device_id);
            boost::remove_erase(midi_inputs, op.device_id);
        }
    };

    auto reduce(state const& st) const -> state override
    {
        auto new_st = st;

        auto midi_devices = *new_st.midi_devices;
        auto midi_inputs = *new_st.midi_inputs;

        algorithm::for_each_visit(
                updates,
                midi_device_update_handler{
                        .midi_devices = midi_devices,
                        .midi_inputs = midi_inputs});

        new_st.midi_devices = std::move(midi_devices);
        new_st.midi_inputs = std::move(midi_inputs);

        return new_st;
    }
};

auto
no_device_updates() -> midi_control_middleware::device_updates_f
{
    return []() { return std::vector<midi::device_update>(); };
}

} // namespace

midi_control_middleware::midi_control_middleware(
        device_updates_f device_updates)
    : m_device_updates(
              device_updates ? std::move(device_updates) : no_device_updates())
{
}

void
midi_control_middleware::operator()(
        middleware_functors const& mw_fs,
        action const& action)
{
    if (auto const* const a =
                dynamic_cast<actions::midi_control_action const*>(&action))
    {
        auto v = ui::make_action_visitor<actions::midi_control_action_visitor>(
                [&](auto const& a) { process_midi_control_action(mw_fs, a); });

        a->visit(v);
    }
    else
    {
        mw_fs.next(action);
    }
}

void
midi_control_middleware::process_device_update(
        middleware_functors const& mw_fs,
        midi::device_added const& up)
{
    if (auto it = std::ranges::find(m_enabled_devices, up.name);
        it != m_enabled_devices.end())
    {
        actions::activate_midi_device action;
        action.device_id = up.device_id;
        mw_fs.dispatch(action);

        m_enabled_devices.erase(it);
        BOOST_ASSERT(!algorithm::contains(m_enabled_devices, up.name));
    }
}

void
midi_control_middleware::process_device_update(
        middleware_functors const& mw_fs,
        midi::device_removed const& up)
{
    auto const& st = mw_fs.get_state();
    if (auto it = st.midi_devices->find(up.device_id);
        it != st.midi_devices->end() && it->second.enabled)
    {
        BOOST_ASSERT(!algorithm::contains(m_enabled_devices, *it->second.name));
        m_enabled_devices.emplace_back(it->second.name);
    }
}

template <>
void
midi_control_middleware::process_midi_control_action(
        middleware_functors const& mw_fs,
        actions::refresh_midi_devices const&)
{
    update_midi_devices next_action;
    next_action.updates = m_device_updates();

    if (!next_action.updates.empty())
    {
        algorithm::for_each_visit(next_action.updates, [&](auto const& up) {
            process_device_update(mw_fs, up);
        });

        mw_fs.next(next_action);
    }
}

template <>
void
midi_control_middleware::process_midi_control_action(
        middleware_functors const& mw_fs,
        actions::save_app_config const& action)
{
    auto next_action = action;

    next_action.enabled_midi_devices = m_enabled_devices;

    for (auto const& [id, config] : *mw_fs.get_state().midi_devices)
    {
        if (config.enabled)
            next_action.enabled_midi_devices.emplace_back(config.name);
    }

    mw_fs.next(next_action);
}

template <>
void
midi_control_middleware::process_midi_control_action(
        middleware_functors const& mw_fs,
        actions::apply_app_config const& action)
{
    auto const& st = mw_fs.get_state();
    for (std::string const& dev_name : action.conf.enabled_midi_input_devices)
    {
        auto it = std::ranges::find_if(
                *st.midi_devices,
                [&](auto const& id_config) {
                    return std::get<1>(id_config).name == dev_name;
                });

        if (it != st.midi_devices->end())
        {
            if (!it->second.enabled)
            {
                actions::activate_midi_device activate_action;
                activate_action.device_id = it->first;
                mw_fs.dispatch(activate_action);
            }
        }
        else
        {
            m_enabled_devices.emplace_back(dev_name);
        }
    }

    std::ranges::sort(m_enabled_devices);
    boost::unique_erase(m_enabled_devices);

    mw_fs.next(action);
}

template <>
void
midi_control_middleware::process_midi_control_action(
        middleware_functors const& mw_fs,
        actions::request_info_update const& action)
{
    mw_fs.next(action);

    auto const& st = mw_fs.get_state();
    actions::request_parameters_update param_update;

    algorithm::for_each_visit(
            st.midi_assignments.get() | std::views::keys,
            [&param_update](auto const& id) { param_update.push_back(id); });

    if (!param_update.empty())
        mw_fs.next(param_update);
}

} // namespace piejam::runtime
