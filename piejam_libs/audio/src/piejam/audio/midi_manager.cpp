// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/midi_manager.h>

#include <piejam/audio/alsa/midi.h>
#include <piejam/entity_id_hash.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <unordered_map>

namespace piejam::audio
{

struct midi_manager::impl
{
    auto process_midi_device_update(alsa::midi_device_added const& op)
            -> midi_device_update
    {
        auto new_id = midi_device_id_t::generate();
        alsa_midi_input_devices.emplace(new_id, op.device);
        return midi_device_added{
                .device_id = new_id,
                .name = fmt::format("{}:{}", op.device.name, op.device.port)};
    }

    auto process_midi_device_update(alsa::midi_device_removed const& op)
            -> midi_device_update
    {
        auto it = std::ranges::find_if(
                alsa_midi_input_devices,
                [&op](auto const& d) { return d.second == op.device; });

        if (it != alsa_midi_input_devices.end())
        {
            auto const device_id = it->first;
            alsa_midi_input_devices.erase(it);
            return midi_device_removed{.device_id = device_id};
        }

        return nullptr;
    }

    alsa::midi_io midi_io;
    alsa::midi_devices alsa_midi_device_manager{
            midi_io.client_id(),
            midi_io.in_port()};

    std::unordered_map<midi_device_id_t, alsa::midi_device>
            alsa_midi_input_devices;
};

midi_manager::midi_manager()
    : m_impl(std::make_unique<impl>())
{
}

midi_manager::~midi_manager() = default;

bool
midi_manager::enable_input_device(midi_device_id_t const& device_id)
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
midi_manager::disable_input_device(midi_device_id_t const& device_id)
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
midi_manager::update_devices() -> std::vector<midi_device_update>
{
    std::vector<midi_device_update> result;

    std::ranges::transform(
            m_impl->alsa_midi_device_manager.update(),
            std::back_inserter(result),
            [this](auto const& update_op) {
                return std::visit(
                        [this](auto const& op) {
                            return m_impl->process_midi_device_update(op);
                        },
                        update_op);
            });

    return result;
}

} // namespace piejam::audio
