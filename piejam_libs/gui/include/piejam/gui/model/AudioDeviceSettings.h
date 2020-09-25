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

#include <piejam/gui/model/StringList.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class AudioSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(StringList* inputDevices READ inputDevices NOTIFY
                       inputDevicesChanged FINAL)
    Q_PROPERTY(StringList* outputDevices READ outputDevices NOTIFY
                       outputDevicesChanged FINAL)
    Q_PROPERTY(StringList* samplerates READ samplerates NOTIFY
                       sampleratesChanged FINAL)
    Q_PROPERTY(StringList* periodSizes READ periodSizes NOTIFY
                       periodSizesChanged FINAL)

public:
    Q_INVOKABLE virtual void subscribe() = 0;
    Q_INVOKABLE virtual void unsubscribe() = 0;

    auto inputDevices() -> StringList* { return &m_inputDevices; }
    auto outputDevices() -> StringList* { return &m_outputDevices; }
    auto samplerates() -> StringList* { return &m_samplerates; }
    auto periodSizes() -> StringList* { return &m_periodSizes; }

    virtual Q_INVOKABLE void refreshDeviceLists() = 0;
    virtual Q_INVOKABLE void selectInputDevice(unsigned index) = 0;
    virtual Q_INVOKABLE void selectOutputDevice(unsigned index) = 0;
    virtual Q_INVOKABLE void selectSamplerate(unsigned index) = 0;
    virtual Q_INVOKABLE void selectPeriodSize(unsigned index) = 0;

signals:

    void inputDevicesChanged();
    void outputDevicesChanged();
    void sampleratesChanged();
    void periodSizesChanged();

private:
    StringList m_inputDevices;
    StringList m_outputDevices;
    StringList m_samplerates;
    StringList m_periodSizes;
};

} // namespace piejam::gui::model
