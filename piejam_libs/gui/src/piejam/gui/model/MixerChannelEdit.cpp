// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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
         runtime::mixer::channel_id const id)
        : in{store_dispatch,
             state_change_subscriber,
             id,
             runtime::mixer::io_socket::in}
        , out{store_dispatch,
              state_change_subscriber,
              id,
              runtime::mixer::io_socket::out}
    {
    }

    AudioRouting in;
    AudioRouting out;
};

MixerChannelEdit::MixerChannelEdit(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : MixerChannel{store_dispatch, state_change_subscriber, id}
    , m_impl(std::make_unique<Impl>(
              store_dispatch,
              state_change_subscriber,
              id))
{
}

MixerChannelEdit::~MixerChannelEdit() = default;

auto
MixerChannelEdit::in() const noexcept -> AudioRouting*
{
    return &m_impl->in;
}

auto
MixerChannelEdit::out() const noexcept -> AudioRouting*
{
    return &m_impl->out;
}

void
MixerChannelEdit::onSubscribe()
{
    MixerChannel::onSubscribe();

    observe(runtime::selectors::make_mixer_channel_can_move_left_selector(
                    channel_id()),
            [this](bool const x) { setCanMoveLeft(x); });

    observe(runtime::selectors::make_mixer_channel_can_move_right_selector(
                    channel_id()),
            [this](bool const x) { setCanMoveRight(x); });
}

void
MixerChannelEdit::changeName(QString const& name)
{
    runtime::actions::set_mixer_channel_name action;
    action.channel_id = channel_id();
    action.name = name.toStdString();
    dispatch(action);
}

void
MixerChannelEdit::changeColor(MaterialColor color)
{
    runtime::actions::set_mixer_channel_color action;
    action.channel_id = channel_id();
    action.color = static_cast<runtime::material_color>(color);
    dispatch(action);
}

void
MixerChannelEdit::moveLeft()
{
    BOOST_ASSERT(canMoveLeft());
    runtime::actions::move_mixer_channel_left action;
    action.channel_id = channel_id();
    dispatch(action);
}

void
MixerChannelEdit::moveRight()
{
    BOOST_ASSERT(canMoveRight());
    runtime::actions::move_mixer_channel_right action;
    action.channel_id = channel_id();
    dispatch(action);
}

void
MixerChannelEdit::deleteChannel()
{
    runtime::actions::delete_mixer_channel action;
    action.mixer_channel_id = channel_id();
    dispatch(action);
}

} // namespace piejam::gui::model
