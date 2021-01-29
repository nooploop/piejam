// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/midi/manager.h>

#include "alsa.h"

#include <piejam/algorithm/contains.h>
#include <piejam/entity_id_hash.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <unordered_map>

namespace piejam::midi
{

struct midi_manager::impl
{
    bool is_update_relevant(alsa::midi_device_added const&) const
    {
        return true;
    }

    bool is_update_relevant(alsa::midi_device_removed const& op) const
    {
        return algorithm::contains_if(
                alsa_midi_input_devices,
                [&op](auto const& d) { return d.second == op.device; });
    }

    bool is_update_relevant(alsa::midi_device_update const& op) const
    {
        return std::visit(
                [this](auto const& op) { return is_update_relevant(op); },
                op);
    }

    auto process_midi_device_update(alsa::midi_device_added const& op)
            -> device_update
    {
        auto new_id = device_id_t::generate();
        alsa_midi_input_devices.emplace(new_id, op.device);
        return device_added{
                .device_id = new_id,
                .name = fmt::format("{}:{}", op.device.name, op.device.port)};
    }

    auto process_midi_device_update(alsa::midi_device_removed const& op)
            -> device_update
    {
        auto it = std::ranges::find_if(
                alsa_midi_input_devices,
                [&op](auto const& d) { return d.second == op.device; });

        BOOST_ASSERT(it != alsa_midi_input_devices.end());

        auto const device_id = it->first;
        alsa_midi_input_devices.erase(it);
        return device_removed{.device_id = device_id};
    }

    auto process_midi_device_update(alsa::midi_device_update const& op)
    {
        return std::visit(
                [this](auto const& op) {
                    return process_midi_device_update(op);
                },
                op);
    }

    alsa::midi_io midi_io;
    alsa::midi_devices alsa_midi_device_manager{
            midi_io.client_id(),
            midi_io.in_port()};

    std::unordered_map<device_id_t, alsa::midi_device> alsa_midi_input_devices;
};

midi_manager::midi_manager()
    : m_impl(std::make_unique<impl>())
{
}

midi_manager::~midi_manager() = default;

bool
midi_manager::activate_input_device(device_id_t const& device_id)
{
    if (auto it = m_impl->alsa_midi_input_devices.find(device_id);
        it != m_impl->alsa_midi_input_devices.end())
    {
        return m_impl->alsa_midi_device_manager.connect_input(
                it->second.client_id,
                it->second.port);
    }

    return false;
}

void
midi_manager::deactivate_input_device(device_id_t const& device_id)
{
    if (auto it = m_impl->alsa_midi_input_devices.find(device_id);
        it != m_impl->alsa_midi_input_devices.end())
    {
        m_impl->alsa_midi_device_manager.disconnect_input(
                it->second.client_id,
                it->second.port);
    }
}

auto
midi_manager::update_devices() -> std::vector<device_update>
{
    std::vector<device_update> result;

    for (auto const& op : m_impl->alsa_midi_device_manager.update())
    {
        if (m_impl->is_update_relevant(op))
            result.emplace_back(m_impl->process_midi_device_update(op));
    }

    return result;
}

} // namespace piejam::midi
