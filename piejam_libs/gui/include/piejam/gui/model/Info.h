// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <QList>
#include <QStringList>

#include <memory>
#include <string>

namespace piejam::gui::model
{

class Info final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(double audioLoad READ audioLoad NOTIFY audioLoadChanged FINAL)
    Q_PROPERTY(unsigned xruns READ xruns NOTIFY xrunsChanged FINAL)
    Q_PROPERTY(QList<float> cpuLoad READ cpuLoad NOTIFY cpuLoadChanged FINAL)
    Q_PROPERTY(int cpuTemp READ cpuTemp NOTIFY cpuTempChanged FINAL)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged FINAL)
    Q_PROPERTY(bool midiLearn READ midiLearn NOTIFY midiLearnChanged FINAL)
    Q_PROPERTY(QStringList logData READ logData NOTIFY logDataChanged FINAL)
    Q_PROPERTY(
            QString logMessage READ logMessage NOTIFY logMessageChanged FINAL)
    Q_PROPERTY(int diskUsage READ diskUsage NOTIFY diskUsageChanged FINAL)

public:
    Info(runtime::store_dispatch, runtime::subscriber&);

    auto audioLoad() const noexcept -> double
    {
        return m_audioLoad;
    }

    void setAudioLoad(double audioLoad)
    {
        if (std::abs(m_audioLoad - audioLoad) > 1.e-3)
        {
            m_audioLoad = audioLoad;
            emit audioLoadChanged();
        }
    }

    auto xruns() const noexcept -> unsigned
    {
        return m_xruns;
    }

    void setXRuns(unsigned xruns)
    {
        if (m_xruns != xruns)
        {
            m_xruns = xruns;
            emit xrunsChanged();
        }
    }

    auto cpuLoad() const noexcept -> QList<float>
    {
        return m_cpuLoad;
    }

    void setCpuLoad(QList<float> x)
    {
        if (m_cpuLoad != x)
        {
            m_cpuLoad = x;
            emit cpuLoadChanged();
        }
    }

    auto cpuTemp() const noexcept -> int
    {
        return m_cpuTemp;
    }

    void setCpuTemp(int x)
    {
        if (m_cpuTemp != x)
        {
            m_cpuTemp = x;
            emit cpuTempChanged();
        }
    }

    auto recording() const noexcept -> bool
    {
        return m_recording;
    }

    void setRecording(bool x)
    {
        if (m_recording != x)
        {
            m_recording = x;
            emit recordingChanged();
        }
    }

    Q_INVOKABLE void changeRecording(bool);

    auto midiLearn() const noexcept -> bool
    {
        return m_midiLearn;
    }

    void setMidiLearn(bool x)
    {
        if (m_midiLearn != x)
        {
            m_midiLearn = x;
            emit midiLearnChanged();
        }
    }

    auto logData() const -> QStringList
    {
        return m_logData;
    }

    void addLogMessage(QString const& msg)
    {
        m_logData.push_back(msg);
        emit logDataChanged();

        if (m_logMessage != msg)
        {
            m_logMessage = msg;
            emit logMessageChanged();
        }
    }

    auto logMessage() const -> QString
    {
        return m_logMessage;
    }

    auto diskUsage() const noexcept -> int
    {
        return m_diskUsage;
    }

    void setDiskUsage(int const x)
    {
        if (m_diskUsage != x)
        {
            m_diskUsage = x;
            emit diskUsageChanged();
        }
    }

signals:

    void audioLoadChanged();
    void xrunsChanged();
    void cpuLoadChanged();
    void cpuTempChanged();
    void recordingChanged();
    void midiLearnChanged();
    void logDataChanged();
    void logMessageChanged();
    void diskUsageChanged();

private:
    void onSubscribe() override;

    double m_audioLoad{};
    unsigned m_xruns{};
    QList<float> m_cpuLoad;
    int m_cpuTemp{};
    bool m_recording{};
    bool m_midiLearn{};
    QStringList m_logData;
    QString m_logMessage;
    int m_diskUsage{};
};

} // namespace piejam::gui::model
