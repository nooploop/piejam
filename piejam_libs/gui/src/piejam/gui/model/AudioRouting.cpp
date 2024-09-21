// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioRouting.h>

#include <piejam/gui/model/AudioRoutingSelection.h>

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::gui::model
{

struct AudioRouting::Impl
{
    runtime::mixer::channel_id mixer_channel_id;
    runtime::mixer::io_socket io_socket;
    audio::bus_type bus_type;

    std::unique_ptr<AudioRoutingSelection> selected;

    boxed_vector<runtime::selectors::mixer_device_route> devices;
    boxed_vector<runtime::selectors::mixer_channel_route> channels;
};

AudioRouting::AudioRouting(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id,
        runtime::mixer::io_socket const io_socket)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl{std::make_unique<Impl>(
              id,
              io_socket,
              observe_once(
                      runtime::selectors::make_mixer_channel_bus_type_selector(
                              id)),
              std::make_unique<AudioRoutingSelection>(
                      store_dispatch,
                      state_change_subscriber,
                      id,
                      io_socket))}
    , m_defaultName{QString::fromStdString(observe_once(
              runtime::selectors::
                      make_mixer_channel_default_route_name_selector(
                              m_impl->bus_type,
                              io_socket)))}
{
    connectSubscribableChild(*m_impl->selected);
}

AudioRouting::~AudioRouting() = default;

auto
AudioRouting::selected() const noexcept -> AudioRoutingSelection*
{
    return m_impl->selected.get();
}

void
AudioRouting::onSubscribe()
{
    observe(runtime::selectors::
                    make_mixer_channel_default_route_is_valid_selector(
                            m_impl->mixer_channel_id,
                            m_impl->io_socket),
            [this](bool const x) { setDefaultIsValid(x); });

    observe(runtime::selectors::make_mixer_device_routes_selector(
                    m_impl->io_socket != runtime::mixer::io_socket::in
                            ? audio::bus_type::stereo
                            : m_impl->bus_type,
                    m_impl->io_socket),
            [this](boxed_vector<runtime::selectors::mixer_device_route> const&
                           devices) {
                m_impl->devices = devices;

                QStringList deviceNames;
                std::ranges::transform(
                        *devices,
                        std::back_inserter(deviceNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_device_route::name);
                setDevices(std::move(deviceNames));
            });

    observe(runtime::selectors::make_mixer_channel_routes_selector(
                    m_impl->mixer_channel_id,
                    m_impl->io_socket),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           channels) {
                m_impl->channels = channels;

                QStringList channelNames;
                std::ranges::transform(
                        *channels,
                        std::back_inserter(channelNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_channel_route::name);
                setChannels(std::move(channelNames));
            });
}

void
AudioRouting::changeToDefault()
{
    runtime::actions::set_mixer_channel_route action;
    action.channel_id = m_impl->mixer_channel_id;
    action.io_socket = m_impl->io_socket;
    dispatch(action);
}

void
AudioRouting::changeToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->devices->size());

    runtime::actions::set_mixer_channel_route action;
    action.channel_id = m_impl->mixer_channel_id;
    action.io_socket = m_impl->io_socket;
    action.route = (*m_impl->devices)[index].device_id;
    dispatch(action);
}

void
AudioRouting::changeToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->channels->size());

    runtime::actions::set_mixer_channel_route action;
    action.channel_id = m_impl->mixer_channel_id;
    action.io_socket = m_impl->io_socket;
    action.route = (*m_impl->channels)[index].channel_id;
    dispatch(action);
}

} // namespace piejam::gui::model
