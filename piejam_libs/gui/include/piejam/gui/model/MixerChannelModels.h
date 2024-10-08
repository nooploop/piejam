// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

namespace piejam::gui::model
{

class MixerChannelModels final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::MixerChannelPerform*,
            perform)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::MixerChannelEdit*, edit)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::MixerChannelFx*, fx)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::MixerChannelAuxSend*,
            auxSend)

public:
    MixerChannelModels(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
