// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelEdit.h>

#include <piejam/gui/model/AudioRouting.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::gui::model
{

struct MixerChannelEdit::Impl
{
    Impl(runtime::store_dispatch store_dispatch,
         runtime::subscriber& state_change_subscriber,
         runtime::mixer::channel_id const id,
         audio::bus_type const bus_type)
        : channel_id{id}
        , bus_type{bus_type}
        , in{store_dispatch,
             state_change_subscriber,
             id,
             runtime::mixer::io_socket::in}
        , out{store_dispatch,
              state_change_subscriber,
              id,
              runtime::mixer::io_socket::out}
    {
    }

    runtime::mixer::channel_id channel_id;
    audio::bus_type bus_type;

    AudioRouting in;
    AudioRouting out;
};

MixerChannelEdit::MixerChannelEdit(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              store_dispatch,
              state_change_subscriber,
              id,
              observe_once(
                      runtime::selectors::make_mixer_channel_bus_type_selector(
                              id))))
    , m_busType(toBusType(m_impl->bus_type))
{
}

MixerChannelEdit::~MixerChannelEdit() = default;

auto
MixerChannelEdit::in() const -> AudioRouting*
{
    return &m_impl->in;
}

auto
MixerChannelEdit::out() const -> AudioRouting*
{
    return &m_impl->out;
}

void
MixerChannelEdit::onSubscribe()
{
    observe(runtime::selectors::make_mixer_channel_name_selector(
                    m_impl->channel_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_mixer_channel_can_move_left_selector(
                    m_impl->channel_id),
            [this](bool const x) { setCanMoveLeft(x); });

    observe(runtime::selectors::make_mixer_channel_can_move_right_selector(
                    m_impl->channel_id),
            [this](bool const x) { setCanMoveRight(x); });
}

void
MixerChannelEdit::changeName(QString const& name)
{
    runtime::actions::set_mixer_channel_name action;
    action.channel_id = m_impl->channel_id;
    action.name = name.toStdString();
    dispatch(action);
}

void
MixerChannelEdit::moveLeft()
{
    BOOST_ASSERT(canMoveLeft());
    runtime::actions::move_mixer_channel_left action;
    action.channel_id = m_impl->channel_id;
    dispatch(action);
}

void
MixerChannelEdit::moveRight()
{
    BOOST_ASSERT(canMoveRight());
    runtime::actions::move_mixer_channel_right action;
    action.channel_id = m_impl->channel_id;
    dispatch(action);
}

void
MixerChannelEdit::deleteChannel()
{
    dispatch(runtime::actions::initiate_mixer_channel_deletion(
            m_impl->channel_id));
}

} // namespace piejam::gui::model
