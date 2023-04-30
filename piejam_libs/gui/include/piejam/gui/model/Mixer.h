// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class Mixer final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* inputChannels READ inputChannels CONSTANT)
    Q_PROPERTY(piejam::gui::model::MixerChannel* mainChannel READ mainChannel
                       NOTIFY mainChannelChanged FINAL)
    Q_PROPERTY(QAbstractListModel* fxChains READ fxChains CONSTANT)

public:
    Mixer(runtime::store_dispatch, runtime::subscriber&);
    ~Mixer();

    auto inputChannels() -> MixerChannelsList*;

    auto mainChannel() const -> MixerChannel*;

    Q_INVOKABLE void addMonoChannel(QString const& newChannelName);
    Q_INVOKABLE void addStereoChannel(QString const& newChannelName);

    Q_INVOKABLE void requestLevelsUpdate();

    auto fxChains() -> FxChainList*;

signals:

    void mainChannelChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> const m_impl;
};

} // namespace piejam::gui::model
