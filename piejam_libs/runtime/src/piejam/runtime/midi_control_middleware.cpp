// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/midi_control_middleware.h>

#include <piejam/midi/device_update.h>
#include <piejam/runtime/actions/refresh_midi_devices.h>
#include <piejam/runtime/state.h>

#include <boost/range/algorithm_ext/erase.hpp>

namespace piejam::runtime
{

namespace
{

struct update_midi_devices final
    : ui::cloneable_action<update_midi_devices, action>
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

        for (auto const& update : updates)
        {
            std::visit(
                    midi_device_update_handler{
                            .midi_devices = midi_devices,
                            .midi_inputs = midi_inputs},
                    update);
        }

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
        middleware_functors mw_fs,
        device_updates_f device_updates)
    : middleware_functors(std::move(mw_fs))
    , m_device_updates(
              device_updates ? std::move(device_updates) : no_device_updates())
{
}

void
midi_control_middleware::operator()(action const& a)
{
    if (typeid(a) == typeid(actions::refresh_midi_devices))
    {
        refresh_midi_devices();
    }
    else
    {
        next(a);
    }
}

void
midi_control_middleware::refresh_midi_devices()
{
    update_midi_devices next_action;
    next_action.updates = m_device_updates();

    if (!next_action.updates.empty())
        next(next_action);
}

} // namespace piejam::runtime
