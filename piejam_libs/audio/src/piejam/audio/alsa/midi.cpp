// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/alsa/midi.h>

#include <spdlog/spdlog.h>

#include <sound/asequencer.h>
#include <sound/asound.h>
#include <sys/ioctl.h>

#include <algorithm>

namespace piejam::audio::alsa
{

namespace
{

auto
open_seq() -> system::device
{
    system::device seq("/dev/snd/seq");

    int version{};
    if (auto err = seq.ioctl(SNDRV_SEQ_IOCTL_PVERSION, version))
        throw std::system_error(err);

    if (SNDRV_PROTOCOL_INCOMPATIBLE(SNDRV_SEQ_VERSION, version))
        throw std::runtime_error("midi seq incompatible version");

    return seq;
}

auto
get_client_id(system::device& seq) -> int
{
    int client_id{};

    if (auto err = seq.ioctl(SNDRV_SEQ_IOCTL_CLIENT_ID, client_id))
        throw std::system_error(err);

    return client_id;
}

auto
make_input_port(system::device& seq, midi_client_id_t client_id) -> midi_port_t
{
    snd_seq_port_info port_info{};
    port_info.addr.client = client_id;
    port_info.type = SNDRV_SEQ_PORT_TYPE_APPLICATION;
    port_info.capability =
            SNDRV_SEQ_PORT_CAP_WRITE | SNDRV_SEQ_PORT_CAP_SUBS_WRITE;

    if (auto err = seq.ioctl(SNDRV_SEQ_IOCTL_CREATE_PORT, port_info))
        throw std::system_error(err);

    return port_info.addr.port;
}

template <class Handler>
void
scan_devices(system::device& seq, Handler&& handler)
{
    snd_seq_client_info client_info{};
    client_info.client = SNDRV_SEQ_CLIENT_SYSTEM;

    while (!seq.ioctl(SNDRV_SEQ_IOCTL_QUERY_NEXT_CLIENT, client_info))
    {
        if (client_info.client == SNDRV_SEQ_CLIENT_SYSTEM)
            continue;

        if (client_info.num_ports <= 0)
            continue;

        snd_seq_port_info port_info{};
        port_info.addr.client = client_info.client;
        port_info.addr.port = 0;

        if (seq.ioctl(SNDRV_SEQ_IOCTL_GET_PORT_INFO, port_info))
            continue;

        do
        {
            std::forward<Handler>(handler)(client_info, port_info);
        } while (!seq.ioctl(SNDRV_SEQ_IOCTL_QUERY_NEXT_PORT, port_info));
    }
}

auto
scan_input_devices(system::device& seq) -> std::vector<midi_device>
{
    std::vector<midi_device> result;

    scan_devices(
            seq,
            [&result](
                    snd_seq_client_info const& client_info,
                    snd_seq_port_info const& port_info) {
                if (client_info.type != KERNEL_CLIENT)
                    return;

                if ((port_info.capability & SNDRV_SEQ_PORT_CAP_READ) &&
                    (port_info.capability & SNDRV_SEQ_PORT_CAP_SUBS_READ))
                {
                    result.emplace_back(
                            port_info.addr.client,
                            port_info.addr.port,
                            client_info.name);
                }
            });

    return result;
}

} // namespace

midi_io::midi_io()
    : m_seq(open_seq())
    , m_client_id(get_client_id(m_seq))
    , m_in_port(make_input_port(m_seq, m_client_id))
    , m_input_buffer(sizeof(snd_seq_event) * 128)
{
    if (auto err = m_seq.set_nonblock())
        throw std::system_error(err);
}

void
midi_io::process_input(midi_message_handler& handler)
{
    auto read_result = m_seq.read(m_input_buffer);
    if (!read_result)
        return;

    std::span<snd_seq_event const> events(
            reinterpret_cast<snd_seq_event const*>(m_input_buffer.data()),
            read_result.value() % sizeof(snd_seq_event));

    for (snd_seq_event const& ev : events)
    {
        switch (ev.type)
        {
            case SNDRV_SEQ_EVENT_CONTROLLER:
                handler.on_controller_event(
                        ev.source.client,
                        ev.source.port,
                        ev.data.control.channel,
                        ev.data.control.param,
                        ev.data.control.value);
                break;

            default:
                break;
        }
    }
}

midi_devices::midi_devices(midi_client_id_t in_client_id, midi_port_t in_port)
    : m_in_client_id(in_client_id)
    , m_in_port(in_port)
    , m_seq(open_seq())
    , m_client_id(get_client_id(m_seq))
    , m_port(make_input_port(m_seq, m_client_id))
{
    snd_seq_port_subscribe port_sub{};
    port_sub.sender.client = SNDRV_SEQ_CLIENT_SYSTEM;
    port_sub.sender.port = SNDRV_SEQ_PORT_SYSTEM_ANNOUNCE;
    port_sub.dest.client = m_client_id;
    port_sub.dest.port = m_port;

    if (auto err = m_seq.ioctl(SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT, port_sub))
        throw std::system_error(err);

    std::ranges::transform(
            scan_input_devices(m_seq),
            std::back_inserter(m_initial_updates),
            [](auto const& d) { return midi_device_added{.device = d}; });

    if (auto err = m_seq.set_nonblock())
        throw std::system_error(err);
}

midi_devices::~midi_devices()
{
    snd_seq_port_subscribe port_sub{};
    port_sub.sender.client = SNDRV_SEQ_CLIENT_SYSTEM;
    port_sub.sender.port = SNDRV_SEQ_PORT_SYSTEM_ANNOUNCE;
    port_sub.dest.client = m_client_id;
    port_sub.dest.port = m_port;

    if (auto err = m_seq.ioctl(SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT, port_sub))
        spdlog::error(
                "midi_manager: failed to unsubscribe from system "
                "announcements: {}",
                err.message());
}

bool
midi_devices::connect_input(
        midi_client_id_t source_client_id,
        midi_port_t source_port)
{
    snd_seq_port_subscribe port_sub{};
    port_sub.sender.client = source_client_id;
    port_sub.sender.port = source_port;
    port_sub.dest.client = m_in_client_id;
    port_sub.dest.port = m_in_port;

    return !m_seq.ioctl(SNDRV_SEQ_IOCTL_SUBSCRIBE_PORT, port_sub);
}

void
midi_devices::disconnect_input(
        midi_client_id_t source_client_id,
        midi_port_t source_port)
{
    snd_seq_port_subscribe port_sub{};
    port_sub.sender.client = source_client_id;
    port_sub.sender.port = source_port;
    port_sub.dest.client = m_in_client_id;
    port_sub.dest.port = m_in_port;

    if (auto err = m_seq.ioctl(SNDRV_SEQ_IOCTL_UNSUBSCRIBE_PORT, port_sub))
        spdlog::warn(
                "midi_manager: disconnect from input failed: {}",
                err.message());
}

auto
midi_devices::update() -> std::vector<midi_device_update>
{
    std::vector<midi_device_update> result = std::move(m_initial_updates);

    snd_seq_event ev{};
    while (auto read_result =
                   m_seq.read({reinterpret_cast<std::byte*>(&ev), sizeof(ev)}))
    {
        if (read_result.value() != sizeof(ev))
        {
            spdlog::critical("midi_devices: failed to read announcement event");
            return result;
        }

        switch (ev.type)
        {
            case SNDRV_SEQ_EVENT_PORT_START:
            {
                snd_seq_port_info port_info{};
                port_info.addr = ev.data.addr;

                if (!m_seq.ioctl(SNDRV_SEQ_IOCTL_GET_PORT_INFO, port_info))
                {
                    if ((port_info.capability & SNDRV_SEQ_PORT_CAP_READ) &&
                        (port_info.capability & SNDRV_SEQ_PORT_CAP_SUBS_READ))
                    {
                        snd_seq_client_info client_info{};
                        client_info.client = port_info.addr.client;
                        if (!m_seq.ioctl(
                                    SNDRV_SEQ_IOCTL_GET_CLIENT_INFO,
                                    client_info))
                        {
                            if (client_info.type == KERNEL_CLIENT)
                            {
                                result.emplace_back(midi_device_added{
                                        .device = midi_device{
                                                .client_id =
                                                        port_info.addr.client,
                                                .port = port_info.addr.port,
                                                .name = client_info.name}});
                            }
                        }
                    }
                }
                break;
            }

            case SNDRV_SEQ_EVENT_PORT_EXIT:
                result.emplace_back(midi_device_removed{
                        .device = midi_device{
                                .client_id = ev.data.addr.client,
                                .port = ev.data.addr.port,
                                .name = {}}});
                break;

            default:
                break;
        }
    }

    return result;
}

} // namespace piejam::audio::alsa
