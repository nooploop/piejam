// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class UISettings final : public CompositeSubscribableModel
{
    Q_OBJECT

    PIEJAM_GUI_CONSTANT_PROPERTY(QAbstractListModel*, displayRotations)
    PIEJAM_GUI_PROPERTY(int, displayRotation, setDisplayRotation)
    PIEJAM_GUI_PROPERTY(
        bool,
        onScreenKeyboardEnabled,
        setOnScreenKeyboardEnabled)

public:
    explicit UISettings(runtime::state_access const&);

    Q_INVOKABLE void selectDisplayRotation(unsigned index);
    Q_INVOKABLE void toggleOnScreenKeyboardEnabled(bool enabled);

private:
    void onSubscribe() override;
};

} // namespace piejam::gui::model
