// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioRouting.h>

#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/AudioRoutingSelection.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/String.h>

#include <piejam/algorithm/edit_script.h>
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
    Strings devicesList{};
    Strings channelsList{};

    boxed_vector<runtime::selectors::mixer_device_route> devices{};
    boxed_vector<runtime::selectors::mixer_channel_route> channels{};
};

AudioRouting::AudioRouting(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id,
        runtime::mixer::io_socket const io_socket)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl{make_pimpl<Impl>(
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
{
    connectSubscribableChild(*m_impl->selected);
}

auto
AudioRouting::selected() const noexcept -> AudioRoutingSelection*
{
    return m_impl->selected.get();
}

auto
AudioRouting::devices() const noexcept -> QAbstractListModel*
{
    return &m_impl->devicesList;
}

auto
AudioRouting::channels() const noexcept -> QAbstractListModel*
{
    return &m_impl->channelsList;
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
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->devices, *devices),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->devicesList,
                                [this](auto const& route) {
                                    return std::make_unique<String>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            route.name);
                                }});

                m_impl->devices = devices;
            });

    observe(runtime::selectors::make_mixer_channel_routes_selector(
                    m_impl->mixer_channel_id,
                    m_impl->io_socket),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           channels) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->channels, *channels),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->channelsList,
                                [this](auto const& route) {
                                    return std::make_unique<String>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            route.name);
                                }});

                m_impl->channels = channels;
            });
}

template <runtime::mixer::io_socket IOSocket>
static void
dispatch_set_mixer_channel_route_action(
        runtime::store_dispatch dispatch,
        runtime::mixer::channel_id channel_id,
        runtime::mixer::io_address_t addr)
{
    runtime::actions::set_mixer_channel_route<IOSocket> action;
    action.channel_id = channel_id;
    action.route = std::move(addr);
    dispatch(action);
}

static void
dispatch_set_mixer_channel_route_action(
        runtime::store_dispatch dispatch,
        runtime::mixer::io_socket io_socket,
        runtime::mixer::channel_id channel_id,
        runtime::mixer::io_address_t addr)
{
    switch (io_socket)
    {
        case runtime::mixer::io_socket::in:
            dispatch_set_mixer_channel_route_action<
                    runtime::mixer::io_socket::in>(dispatch, channel_id, addr);
            return;

        case runtime::mixer::io_socket::out:
            dispatch_set_mixer_channel_route_action<
                    runtime::mixer::io_socket::out>(dispatch, channel_id, addr);
            return;

        case runtime::mixer::io_socket::aux:
            dispatch_set_mixer_channel_route_action<
                    runtime::mixer::io_socket::aux>(dispatch, channel_id, addr);
            return;
    }
}

void
AudioRouting::changeToDefault()
{
    dispatch_set_mixer_channel_route_action(
            dispatch(),
            m_impl->io_socket,
            m_impl->mixer_channel_id,
            default_t{});
}

void
AudioRouting::changeToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->devices->size());

    dispatch_set_mixer_channel_route_action(
            dispatch(),
            m_impl->io_socket,
            m_impl->mixer_channel_id,
            (*m_impl->devices)[index].device_id);
}

void
AudioRouting::changeToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->channels->size());

    dispatch_set_mixer_channel_route_action(
            dispatch(),
            m_impl->io_socket,
            m_impl->mixer_channel_id,
            (*m_impl->channels)[index].channel_id);
}

} // namespace piejam::gui::model
