// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <QObject>

namespace piejam::gui::model
{

class Factory : public QObject
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::AudioDeviceSettings* audioDeviceSettings READ
                       audioDeviceSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::AudioInputOutputSettings* audioInputSettings
                       READ audioInputSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::AudioInputOutputSettings* audioOutputSettings
                       READ audioOutputSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::MidiInputSettings* midiInputSettings READ
                       midiInputSettings CONSTANT)

    Q_PROPERTY(piejam::gui::model::Mixer* mixer READ mixer CONSTANT)
    Q_PROPERTY(piejam::gui::model::Info* info READ info CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxChain* fxChain READ fxChain CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxBrowser* fxBrowser READ fxBrowser CONSTANT)

public:
    virtual auto audioDeviceSettings() const -> AudioDeviceSettings* = 0;
    virtual auto audioInputSettings() const -> AudioInputOutputSettings* = 0;
    virtual auto audioOutputSettings() const -> AudioInputOutputSettings* = 0;

    virtual auto midiInputSettings() const -> MidiInputSettings* = 0;

    virtual auto mixer() const -> Mixer* = 0;
    virtual auto info() const -> Info* = 0;
    virtual auto fxChain() const -> FxChain* = 0;
    virtual auto fxBrowser() const -> FxBrowser* = 0;
};

} // namespace piejam::gui::model
