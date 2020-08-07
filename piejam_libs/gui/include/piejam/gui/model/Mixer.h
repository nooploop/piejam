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
    Q_PROPERTY(QQmlListProperty<MixerChannel> outputChannels READ outputChannels
                       NOTIFY outputChannelsChanged FINAL)

public:
    void setNumInputChannels(std::size_t);
    auto inputChannels() -> QQmlListProperty<MixerChannel>;
    auto inputChannel(std::size_t index) noexcept -> MixerChannel&
    {
        assert(index < m_inputChannels.size());
        return m_inputChannels[index];
    }

    void setNumOutputChannels(std::size_t);
    auto outputChannels() -> QQmlListProperty<MixerChannel>;
    auto outputChannel(std::size_t index) noexcept -> MixerChannel&
    {
        assert(index < m_outputChannels.size());
        return m_outputChannels[index];
    }

    virtual Q_INVOKABLE void
    setInputChannelGain(unsigned index, double gain) = 0;
    virtual Q_INVOKABLE void setInputChannelPan(unsigned index, double pan) = 0;
    virtual Q_INVOKABLE void
    setOutputChannelGain(unsigned index, double gain) = 0;
    virtual Q_INVOKABLE void
    setOutputChannelBalance(unsigned index, double balance) = 0;
    virtual Q_INVOKABLE void requestLevelsUpdate() = 0;

signals:

    void inputChannelsChanged();
    void outputChannelsChanged();

private:
    std::vector<MixerChannel> m_inputChannels;
    std::vector<MixerChannel> m_outputChannels;
};

} // namespace piejam::gui::model
