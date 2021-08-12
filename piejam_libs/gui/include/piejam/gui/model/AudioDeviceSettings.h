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

class AudioDeviceSettings final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::StringList* inputDevices READ inputDevices
                       NOTIFY inputDevicesChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StringList* outputDevices READ outputDevices
                       NOTIFY outputDevicesChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StringList* sampleRates READ sampleRates
                       NOTIFY sampleRatesChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StringList* periodSizes READ periodSizes
                       NOTIFY periodSizesChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StringList* periodCounts READ periodCounts
                       NOTIFY periodCountsChanged FINAL)
    Q_PROPERTY(double bufferLatency READ bufferLatency NOTIFY
                       bufferLatencyChanged FINAL)

public:
    AudioDeviceSettings(runtime::store_dispatch, runtime::subscriber&);
    ~AudioDeviceSettings();

    auto inputDevices() -> StringList*;
    auto outputDevices() -> StringList*;
    auto sampleRates() -> StringList*;
    auto periodSizes() -> StringList*;
    auto periodCounts() -> StringList*;

    auto bufferLatency() const noexcept -> double { return m_bufferLatency; }
    void setBufferLatency(double x)
    {
        if (m_bufferLatency != x)
        {
            m_bufferLatency = x;
            emit bufferLatencyChanged();
        }
    }

    Q_INVOKABLE void refreshDeviceLists();
    Q_INVOKABLE void selectInputDevice(unsigned index);
    Q_INVOKABLE void selectOutputDevice(unsigned index);
    Q_INVOKABLE void selectSamplerate(unsigned index);
    Q_INVOKABLE void selectPeriodSize(unsigned index);
    Q_INVOKABLE void selectPeriodCount(unsigned index);

signals:
    void inputDevicesChanged();
    void outputDevicesChanged();
    void sampleRatesChanged();
    void periodSizesChanged();
    void periodCountsChanged();
    void bufferLatencyChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    double m_bufferLatency{};
};

} // namespace piejam::gui::model
