// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/midi/device_manager.h>

#include "alsa.h"

#include <piejam/algorithm/contains.h>
#include <piejam/entity_id_hash.h>
#include <piejam/midi/event.h>
#include <piejam/midi/event_handler.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/tuple_element_compare.h>

#include <fmt/format.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

#include <algorithm>
#include <unordered_map>

namespace piejam::midi
{

namespace
{

using alsa_midi_devices_t = boost::container::flat_map<
        std::pair<alsa::midi_client_id_t, alsa::midi_port_t>,
        device_id_t>;

struct alsa_event_handler final : alsa::event_handler
{
    alsa_event_handler(
            midi::event_handler& ev_handler,
            alsa_midi_devices_t const& devices)
        : m_ev_handler(ev_handler)
        , m_devices(devices)
    {
    }

    void process_cc_event(
            alsa::midi_client_id_t const client_id,
            alsa::midi_port_t const port,
            std::size_t const channel,
            std::size_t const cc_id,
            std::size_t const value) override
    {
        if (auto it = m_devices.find(std::pair{client_id, port});
            it != m_devices.end())
        {
            m_ev_handler.process(midi::external_event{
                    .device_id = it->second,
                    .event = midi::channel_cc_event{
                            .channel = channel,
                            .data = cc_event{.cc = cc_id, .value = value}}});
        }
    }

private:
    midi::event_handler& m_ev_handler;
    alsa_midi_devices_t const& m_devices;
};

struct alsa_input_processor final : input_event_handler
{
    alsa_input_processor(
            alsa::midi_io& mio,
            std::unordered_map<device_id_t, alsa::midi_device> const& devices)
        : m_midi_io(mio)
    {
        m_devices.reserve(devices.size());

        std::ranges::transform(
                devices,
                std::inserter(m_devices, m_devices.end()),
                [](auto const& d) {
                    return std::pair{
                            std::pair{d.second.client_id, d.second.port},
                            d.first};
                });
    }

    void process(event_handler& ev_handler)
    {
        alsa_event_handler alsa_ev_handler(ev_handler, m_devices);
        m_midi_io.process_input(alsa_ev_handler);
    }

private:
    alsa::midi_io& m_midi_io;
    alsa_midi_devices_t m_devices;
};

class alsa_device_manager final : public device_manager
{
public:
    bool activate_input_device(device_id_t const& device_id) override;
    void deactivate_input_device(device_id_t const& device_id) override;

    auto update_devices() -> std::vector<device_update> override;

    auto make_input_processor() -> std::unique_ptr<input_event_handler> override;

private:
    bool is_update_relevant(alsa::midi_device_added const&) const
    {
        return true;
    }

    bool is_update_relevant(alsa::midi_device_removed const& op) const
    {
        return algorithm::contains_if(
                m_alsa_input_devices,
                tuple::element<1>.equal_to(std::cref(op.device)));
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
        m_alsa_input_devices.emplace(new_id, op.device);
        return device_added{
                .device_id = new_id,
                .name = fmt::format("{}:{}", op.device.name, op.device.port)};
    }

    auto process_midi_device_update(alsa::midi_device_removed const& op)
            -> device_update
    {
        auto it = std::ranges::find_if(
                m_alsa_input_devices,
                tuple::element<1>.equal_to(std::cref(op.device)));

        BOOST_ASSERT(it != m_alsa_input_devices.end());

        auto const device_id = it->first;
        m_alsa_input_devices.erase(it);
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

    alsa::midi_io m_midi_io;
    alsa::midi_devices m_alsa_midi_devices{
            m_midi_io.client_id(),
            m_midi_io.in_port()};

    std::unordered_map<device_id_t, alsa::midi_device> m_alsa_input_devices;
};

bool
alsa_device_manager::activate_input_device(device_id_t const& device_id)
{
    if (auto it = m_alsa_input_devices.find(device_id);
        it != m_alsa_input_devices.end())
    {
        return m_alsa_midi_devices.connect_input(
                it->second.client_id,
                it->second.port);
    }

    return false;
}

void
alsa_device_manager::deactivate_input_device(device_id_t const& device_id)
{
    if (auto it = m_alsa_input_devices.find(device_id);
        it != m_alsa_input_devices.end())
    {
        m_alsa_midi_devices.disconnect_input(
                it->second.client_id,
                it->second.port);
    }
}

auto
alsa_device_manager::update_devices() -> std::vector<device_update>
{
    std::vector<device_update> result;

    for (auto const& op : m_alsa_midi_devices.update())
    {
        if (is_update_relevant(op))
            result.emplace_back(process_midi_device_update(op));
    }

    return result;
}

auto
alsa_device_manager::make_input_processor() -> std::unique_ptr<input_event_handler>
{
    return std::make_unique<alsa_input_processor>(
            m_midi_io,
            m_alsa_input_devices);
}

} // namespace

auto
make_device_manager() -> std::unique_ptr<device_manager>
{
    return std::make_unique<alsa_device_manager>();
}

} // namespace piejam::midi
