// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class Mixer : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* inputChannels READ inputChannels CONSTANT)
    Q_PROPERTY(piejam::gui::model::MixerChannel* mainChannel READ mainChannel
                       NOTIFY mainChannelChanged FINAL)

public:
    auto inputChannels() -> MixerChannelsList* { return &m_inputChannels; }

    virtual auto mainChannel() const -> MixerChannel* = 0;

    virtual Q_INVOKABLE void addChannel(QString const& newChannelName) = 0;

    virtual Q_INVOKABLE void requestLevelsUpdate() = 0;

signals:

    void mainChannelChanged();

private:
    MixerChannelsList m_inputChannels;
};

} // namespace piejam::gui::model
