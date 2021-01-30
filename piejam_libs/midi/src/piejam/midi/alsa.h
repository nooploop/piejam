// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/system/device.h>

#include <string>
#include <variant>
#include <vector>

namespace piejam::midi::alsa
{

using midi_client_id_t = int;
using midi_port_t = int;

class event_handler
{
public:
    virtual ~event_handler() = default;

    virtual void process_cc_event(
            midi_client_id_t,
            midi_port_t,
            std::size_t channel,
            std::size_t cc_id,
            std::size_t value) = 0;
};

class midi_io
{
public:
    midi_io();

    auto client_id() const noexcept -> midi_client_id_t { return m_client_id; }
    auto in_port() const noexcept -> midi_port_t { return m_in_port; }

    void process_input(event_handler&);

private:
    system::device m_seq;

    midi_client_id_t m_client_id{};
    midi_port_t m_in_port{};

    std::vector<std::byte> m_input_buffer;
};

struct midi_device
{
    midi_client_id_t client_id{};
    midi_port_t port{};
    std::string name;

    constexpr bool operator==(midi_device const& other) const noexcept
    {
        return client_id == other.client_id && port == other.port;
    }

    constexpr bool operator!=(midi_device const& other) const noexcept
    {
        return !(*this == other);
    }
};

struct midi_device_added
{
    midi_device device;
};

struct midi_device_removed
{
    midi_device device;
};

using midi_device_update = std::variant<midi_device_added, midi_device_removed>;

class midi_devices
{
public:
    midi_devices(midi_client_id_t in_client_id, midi_port_t in_port);
    ~midi_devices();

    bool connect_input(midi_client_id_t, midi_port_t);
    void disconnect_input(midi_client_id_t, midi_port_t);

    [[nodiscard]] auto update() -> std::vector<midi_device_update>;

private:
    midi_client_id_t m_in_client_id{};
    midi_port_t m_in_port{};

    system::device m_seq;
    midi_client_id_t m_client_id{};
    midi_port_t m_port;

    std::vector<midi_device_update> m_initial_updates;
};

} // namespace piejam::midi::alsa
