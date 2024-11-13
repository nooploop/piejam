// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

class QAbstractListModel;

namespace piejam::gui::model
{

class Mixer final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_CONSTANT_PROPERTY(QAbstractListModel*, userChannels)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            piejam::gui::model::MixerChannelModels*,
            mainChannel)

public:
    Mixer(runtime::store_dispatch, runtime::subscriber&);

    Q_INVOKABLE void addMonoChannel(QString const& newChannelName);
    Q_INVOKABLE void addStereoChannel(QString const& newChannelName);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> const m_impl;
};

} // namespace piejam::gui::model
