// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class Mixer final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* userChannels READ userChannels CONSTANT)
    Q_PROPERTY(piejam::gui::model::MixerChannel* mainChannel READ mainChannel
                       NOTIFY mainChannelChanged FINAL)

public:
    Mixer(runtime::store_dispatch, runtime::subscriber&);
    ~Mixer() override;

    auto userChannels() -> MixerChannelsList*;

    auto mainChannel() const -> MixerChannel*;

    Q_INVOKABLE void addMonoChannel(QString const& newChannelName);
    Q_INVOKABLE void addStereoChannel(QString const& newChannelName);

    Q_INVOKABLE void requestLevelsUpdate();

signals:

    void mainChannelChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> const m_impl;
};

} // namespace piejam::gui::model
