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
    Q_PROPERTY(StringList* sampleRates READ sampleRates NOTIFY
                       sampleRatesChanged FINAL)
    Q_PROPERTY(StringList* periodSizes READ periodSizes NOTIFY
                       periodSizesChanged FINAL)
    Q_PROPERTY(StringList* periodCounts READ periodCounts NOTIFY
                       periodCountsChanged FINAL)
    Q_PROPERTY(double bufferLatency READ bufferLatency NOTIFY
                       bufferLatencyChanged FINAL)

public:
    auto inputDevices() -> StringList* { return &m_inputDevices; }
    auto outputDevices() -> StringList* { return &m_outputDevices; }
    auto sampleRates() -> StringList* { return &m_sampleRates; }
    auto periodSizes() -> StringList* { return &m_periodSizes; }
    auto periodCounts() -> StringList* { return &m_periodCounts; }

    auto bufferLatency() const noexcept -> double { return m_bufferLatency; }
    void setBufferLatency(double x)
    {
        if (m_bufferLatency != x)
        {
            m_bufferLatency = x;
            emit bufferLatencyChanged();
        }
    }

    virtual Q_INVOKABLE void refreshDeviceLists() = 0;
    virtual Q_INVOKABLE void selectInputDevice(unsigned index) = 0;
    virtual Q_INVOKABLE void selectOutputDevice(unsigned index) = 0;
    virtual Q_INVOKABLE void selectSamplerate(unsigned index) = 0;
    virtual Q_INVOKABLE void selectPeriodSize(unsigned index) = 0;
    virtual Q_INVOKABLE void selectPeriodCount(unsigned index) = 0;

signals:
    void inputDevicesChanged();
    void outputDevicesChanged();
    void sampleRatesChanged();
    void periodSizesChanged();
    void periodCountsChanged();
    void bufferLatencyChanged();

private:
    StringList m_inputDevices;
    StringList m_outputDevices;
    StringList m_sampleRates;
    StringList m_periodSizes;
    StringList m_periodCounts;
    double m_bufferLatency{};
};

} // namespace piejam::gui::model
