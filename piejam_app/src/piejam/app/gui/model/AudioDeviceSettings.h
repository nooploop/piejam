// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/AudioDeviceSettings.h>

namespace piejam::app::gui::model
{

class AudioDeviceSettings final
    : public Subscribable<piejam::gui::model::AudioDeviceSettings>
{
public:
    AudioDeviceSettings(runtime::store_dispatch, runtime::subscriber&);

    virtual void refreshDeviceLists() override;
    virtual void selectInputDevice(unsigned index) override;
    virtual void selectOutputDevice(unsigned index) override;
    virtual void selectSamplerate(unsigned index) override;
    virtual void selectPeriodSize(unsigned index) override;
    virtual void selectPeriodCount(unsigned index) override;

private:
    void onSubscribe() override;
};

} // namespace piejam::app::gui::model
