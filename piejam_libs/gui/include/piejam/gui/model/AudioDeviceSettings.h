// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/StringList.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <memory>

namespace piejam::gui::model
{

class AudioDeviceSettings : public SubscribableModel
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
