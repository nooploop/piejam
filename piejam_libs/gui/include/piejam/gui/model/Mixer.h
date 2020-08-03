// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/gui/model/MixerChannel.h>

#include <QObject>
#include <QQmlListProperty>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class Mixer : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<MixerChannel> inputChannels READ inputChannels
                       NOTIFY inputChannelsChanged FINAL)
    Q_PROPERTY(MixerChannel* outputChannel READ outputChannel NOTIFY
                       outputChannelChanged FINAL)

public:
    auto inputChannels() -> QQmlListProperty<MixerChannel>;
    void setNumInputChannels(std::size_t);

    auto numInputChannels() const noexcept -> std::size_t
    {
        return m_inputChannels.size();
    }

    auto inputChannel(std::size_t index) noexcept -> MixerChannel&
    {
        return m_inputChannels[index];
    }

    auto outputChannel() -> MixerChannel* { return &m_outputChannel; }

    virtual Q_INVOKABLE void
    setInputChannelGain(unsigned index, double gain) = 0;
    virtual Q_INVOKABLE void setInputChannelPan(unsigned index, double pan) = 0;
    virtual Q_INVOKABLE void setOutputChannelGain(double gain) = 0;
    virtual Q_INVOKABLE void setOutputChannelBalance(double balance) = 0;
    virtual Q_INVOKABLE void requestLevelsUpdate() = 0;

signals:

    void inputChannelsChanged();
    void outputChannelChanged();

private:
    std::vector<MixerChannel> m_inputChannels;
    MixerChannel m_outputChannel;
};

} // namespace piejam::gui::model
