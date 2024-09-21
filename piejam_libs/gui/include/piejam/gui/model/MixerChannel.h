// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class MixerChannel final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::MixerChannelPerform* perform READ perform
                       CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::MixerChannelEdit* edit READ edit CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::MixerChannelFx* fx READ fx CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::MixerChannelAuxSend* auxSend READ auxSend
                       CONSTANT FINAL)

public:
    MixerChannel(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannel();

    auto perform() const -> MixerChannelPerform*;
    auto edit() const -> MixerChannelEdit*;
    auto fx() const -> MixerChannelFx*;
    auto auxSend() const -> MixerChannelAuxSend*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
