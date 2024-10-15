// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class AudioDeviceSettings final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            piejam::gui::model::StringList*,
            inputSoundCards)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            piejam::gui::model::StringList*,
            outputSoundCards)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(piejam::gui::model::StringList*, sampleRates)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(piejam::gui::model::StringList*, periodSizes)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            piejam::gui::model::StringList*,
            periodCounts)
    M_PIEJAM_GUI_PROPERTY(double, bufferLatency, setBufferLatency)

public:
    AudioDeviceSettings(runtime::store_dispatch, runtime::subscriber&);

    Q_INVOKABLE void refreshSoundCardLists();
    Q_INVOKABLE void selectInputSoundCard(unsigned index);
    Q_INVOKABLE void selectOutputSoundCard(unsigned index);
    Q_INVOKABLE void selectSampleRate(unsigned index);
    Q_INVOKABLE void selectPeriodSize(unsigned index);
    Q_INVOKABLE void selectPeriodCount(unsigned index);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
