// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/MixerChannel.h>

#include <piejam/app/gui/model/MixerChannelEdit.h>
#include <piejam/app/gui/model/MixerChannelPerform.h>

namespace piejam::app::gui::model
{

struct MixerChannel::Impl
{
    std::unique_ptr<MixerChannelPerform> m_perform;
    std::unique_ptr<MixerChannelEdit> m_edit;
};

MixerChannel::MixerChannel(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id,
        runtime::float_parameter_id const volume,
        runtime::float_parameter_id const pan_balance,
        runtime::bool_parameter_id const mute,
        runtime::bool_parameter_id const solo,
        runtime::stereo_level_parameter_id const level)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              std::make_unique<MixerChannelPerform>(
                      store_dispatch,
                      state_change_subscriber,
                      id,
                      volume,
                      pan_balance,
                      mute,
                      solo,
                      level),
              std::make_unique<MixerChannelEdit>(
                      store_dispatch,
                      state_change_subscriber,
                      id)))
{
}

MixerChannel::~MixerChannel() = default;

void
MixerChannel::onSubscribe()
{
}

auto
MixerChannel::perform() const -> piejam::gui::model::MixerChannelPerform*
{
    return m_impl->m_perform.get();
}

auto
MixerChannel::edit() const -> piejam::gui::model::MixerChannelEdit*
{
    return m_impl->m_edit.get();
}

} // namespace piejam::app::gui::model
