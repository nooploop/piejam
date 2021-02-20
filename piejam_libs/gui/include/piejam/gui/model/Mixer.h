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
    Q_PROPERTY(bool inputSoloActive READ inputSoloActive NOTIFY
                       inputSoloActiveChanged FINAL)
    Q_PROPERTY(bool outputSoloActive READ outputSoloActive NOTIFY
                       outputSoloActiveChanged FINAL)

public:
    auto inputChannels() -> MixerChannelsList* { return &m_inputChannels; }

    auto inputSoloActive() const -> bool { return m_inputSoloActive; }
    void setInputSoloActive(bool x)
    {
        if (m_inputSoloActive != x)
        {
            m_inputSoloActive = x;
            emit inputSoloActiveChanged();
        }
    }

    virtual auto mainChannel() const -> MixerChannel* = 0;

    auto outputSoloActive() const -> bool { return m_outputSoloActive; }
    void setOutputSoloActive(bool x)
    {
        if (m_outputSoloActive != x)
        {
            m_outputSoloActive = x;
            emit outputSoloActiveChanged();
        }
    }

    virtual Q_INVOKABLE void addChannel() = 0;

    virtual Q_INVOKABLE void setInputSolo(unsigned index) = 0;
    virtual Q_INVOKABLE void setOutputSolo(unsigned index) = 0;

    virtual Q_INVOKABLE void requestLevelsUpdate() = 0;

signals:

    void mainChannelChanged();
    void inputSoloActiveChanged();
    void outputSoloActiveChanged();

private:
    MixerChannelsList m_inputChannels;
    MixerChannelsList m_outputChannels;
    bool m_inputSoloActive{};
    bool m_outputSoloActive{};
};

} // namespace piejam::gui::model
