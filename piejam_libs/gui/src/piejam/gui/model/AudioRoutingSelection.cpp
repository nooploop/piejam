// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioRoutingSelection.h>

#include <piejam/runtime/selectors.h>

#include <boost/hof/match.hpp>

namespace piejam::gui::model
{

struct AudioRoutingSelection::Impl
{
    runtime::mixer::channel_id mixer_channel_id;
    runtime::mixer::io_socket io_socket;
    audio::bus_type bus_type;

    runtime::subscription_id target_channel_state_sub_id{
            runtime::subscription_id::generate()};
    runtime::subscription_id target_name_sub_id{
            runtime::subscription_id::generate()};
};

AudioRoutingSelection::AudioRoutingSelection(
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
                              id)))}
{
}

void
AudioRoutingSelection::onSubscribe()
{
    using namespace runtime::selectors;
    using namespace runtime::mixer;

    observe(make_mixer_channel_selected_route_selector(
                    m_impl->mixer_channel_id,
                    m_impl->io_socket),
            [this](io_address_t const& io_address) {
                unobserve(m_impl->target_channel_state_sub_id);
                unobserve(m_impl->target_name_sub_id);

                std::visit(
                        boost::hof::match(
                                [this](default_t) {
                                    setDefault(true);
                                    setState(State::Valid);
                                    setLabel(QString());
                                },
                                [this](invalid_t) {
                                    setDefault(true);
                                    setState(State::Invalid);
                                    setLabel(QString());
                                },
                                [this](runtime::external_audio::device_id
                                               device_id) {
                                    setDefault(false);
                                    setState(State::Valid);
                                    observe(m_impl->target_name_sub_id,
                                            make_external_audio_device_name_selector(
                                                    device_id),
                                            [this](boxed_string device_name) {
                                                setLabel(QString::fromStdString(
                                                        device_name));
                                            });
                                },
                                [this](channel_id target_channel) {
                                    setDefault(false);
                                    if (m_impl->io_socket == io_socket::in)
                                    {
                                        setState(State::Valid);
                                    }
                                    else
                                    {
                                        observe(m_impl->target_channel_state_sub_id,
                                                make_mixer_channel_selected_route_selector(
                                                        target_channel,
                                                        io_socket::in),
                                                [this](io_address_t const&
                                                               target_channel_in) {
                                                    setState(
                                                            std::holds_alternative<
                                                                    default_t>(
                                                                    target_channel_in)
                                                                    ? State::Valid
                                                                    : State::NotMixed);
                                                });
                                    }

                                    observe(m_impl->target_name_sub_id,
                                            make_mixer_channel_name_selector(
                                                    target_channel),
                                            [this](boxed_string channel_name) {
                                                setLabel(QString::fromStdString(
                                                        channel_name));
                                            });
                                }),
                        io_address);
            });
}

} // namespace piejam::gui::model
