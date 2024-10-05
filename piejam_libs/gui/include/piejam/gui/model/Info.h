// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>

#include <QList>
#include <QStringList>

namespace piejam::gui::model
{

class Info final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, audioLoad, setAudioLoad)
    M_PIEJAM_GUI_PROPERTY(unsigned, xruns, setXruns)
    M_PIEJAM_GUI_PROPERTY(QList<float>, cpuLoad, setCpuLoad)
    M_PIEJAM_GUI_PROPERTY(int, cpuTemp, setCpuTemp)
    M_PIEJAM_GUI_PROPERTY(bool, recording, setRecording)
    M_PIEJAM_GUI_PROPERTY(bool, midiLearn, setMidiLearn)
    M_PIEJAM_GUI_PROPERTY(int, diskUsage, setDiskUsage)

public:
    Info(runtime::store_dispatch, runtime::subscriber&);

    Q_INVOKABLE void changeRecording(bool);

private:
    void onSubscribe() override;
};

} // namespace piejam::gui::model
