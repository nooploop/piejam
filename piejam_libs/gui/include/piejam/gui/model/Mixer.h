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
#include <piejam/gui/model/MixerChannelsList.h>

#include <QObject>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class Mixer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(MixerChannelsList* inputChannels READ inputChannels CONSTANT)
    Q_PROPERTY(MixerChannelsList* outputChannels READ outputChannels CONSTANT)
    Q_PROPERTY(bool inputSoloActive READ inputSoloActive NOTIFY
                       inputSoloActiveChanged FINAL)

public:
    auto inputChannels() -> MixerChannelsList* { return &m_inputChannels; }
    auto numInputChannels() const noexcept -> std::size_t
    {
        return m_inputChannels.rowCount();
    }

    auto outputChannels() -> MixerChannelsList* { return &m_outputChannels; }
    auto numOutputChannels() const noexcept -> std::size_t
    {
        return m_outputChannels.rowCount();
    }

    auto inputSoloActive() const -> bool { return m_inputSoloActive; }
    void setInputSoloActive(bool);

    virtual Q_INVOKABLE void setInputSolo(unsigned index) = 0;
    virtual Q_INVOKABLE void requestLevelsUpdate() = 0;

signals:

    void inputSoloActiveChanged();

private:
    MixerChannelsList m_inputChannels;
    MixerChannelsList m_outputChannels;
    bool m_inputSoloActive{};
};

} // namespace piejam::gui::model
