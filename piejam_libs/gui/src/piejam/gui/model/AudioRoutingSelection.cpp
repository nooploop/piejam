// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioRoutingSelection.h>

#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct AudioRoutingSelection::Impl
{
    runtime::mixer::channel_id mixer_channel_id;
    runtime::mixer::io_socket io_socket;
};

AudioRoutingSelection::AudioRoutingSelection(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id,
        runtime::mixer::io_socket const io_socket)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl{std::make_unique<Impl>(id, io_socket)}
{
}

AudioRoutingSelection::~AudioRoutingSelection() = default;

void
AudioRoutingSelection::onSubscribe()
{
    observe(runtime::selectors::make_mixer_channel_selected_route_selector(
                    m_impl->mixer_channel_id,
                    m_impl->io_socket),
            [this](box<runtime::selectors::selected_route> const& sel_route) {
                setLabel(QString::fromStdString(sel_route->name));
                switch (sel_route->state)
                {
                    case runtime::selectors::selected_route::state_t::valid:
                        setState(State::Valid);
                        break;

                    case runtime::selectors::selected_route::state_t::not_mixed:
                        setState(State::NotMixed);
                        break;

                    default:
                        setState(State::Invalid);
                        break;
                }
            });
}

} // namespace piejam::gui::model
