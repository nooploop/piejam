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

    Q_PROPERTY(piejam::gui::model::Mixer* mixer READ mixer CONSTANT)
    Q_PROPERTY(piejam::gui::model::Info* info READ info CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxChain* fxChain READ fxChain CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxBrowser* fxBrowser READ fxBrowser CONSTANT)

public:
    virtual auto audioDeviceSettings() const -> AudioDeviceSettings* = 0;
    virtual auto audioInputSettings() const -> AudioInputOutputSettings* = 0;
    virtual auto audioOutputSettings() const -> AudioInputOutputSettings* = 0;

    virtual auto mixer() const -> Mixer* = 0;
    virtual auto info() const -> Info* = 0;
    virtual auto fxChain() const -> FxChain* = 0;
    virtual auto fxBrowser() const -> FxBrowser* = 0;
};

} // namespace piejam::gui::model
