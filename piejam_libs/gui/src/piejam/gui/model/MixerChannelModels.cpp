// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelModels.h>

#include <piejam/gui/model/MixerChannelAuxSend.h>
#include <piejam/gui/model/MixerChannelEdit.h>
#include <piejam/gui/model/MixerChannelFx.h>
#include <piejam/gui/model/MixerChannelPerform.h>

namespace piejam::gui::model
{

struct MixerChannelModels::Impl
{
    Impl(runtime::store_dispatch store_dispatch,
         runtime::subscriber& state_change_subscriber,
         runtime::mixer::channel_id const id)
        : m_perform{store_dispatch, state_change_subscriber, id}
        , m_edit{store_dispatch, state_change_subscriber, id}
        , m_fx{store_dispatch, state_change_subscriber, id}
        , m_auxSend{store_dispatch, state_change_subscriber, id}
    {
    }

    MixerChannelPerform m_perform;
    MixerChannelEdit m_edit;
    MixerChannelFx m_fx;
    MixerChannelAuxSend m_auxSend;
};

MixerChannelModels::MixerChannelModels(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              store_dispatch,
              state_change_subscriber,
              id))
{
}

MixerChannelModels::~MixerChannelModels() = default;

void
MixerChannelModels::onSubscribe()
{
}

auto
MixerChannelModels::perform() const -> MixerChannelPerform*
{
    return &m_impl->m_perform;
}

auto
MixerChannelModels::edit() const -> MixerChannelEdit*
{
    return &m_impl->m_edit;
}

auto
MixerChannelModels::fx() const -> MixerChannelFx*
{
    return &m_impl->m_fx;
}

auto
MixerChannelModels::auxSend() const -> MixerChannelAuxSend*
{
    return &m_impl->m_auxSend;
}

} // namespace piejam::gui::model
