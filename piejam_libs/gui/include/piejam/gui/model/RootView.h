// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/root_view_mode.h>

namespace piejam::gui::model
{

class RootView final : public Subscribable<SubscribableModel>
{
    Q_OBJECT
public:
    enum class Mode : int
    {
        Mixer,
        Info,
        Settings,
        Power,
        FxBrowser,
        FxModule,
    };

    Q_ENUM(Mode)

private:
    M_PIEJAM_GUI_PROPERTY(bool, canShowFxModule, setCanShowFxModule)
    M_PIEJAM_GUI_PROPERTY(Mode, mode, setMode)

public:
    RootView(runtime::store_dispatch, runtime::subscriber&);
    ~RootView() override;

    Q_INVOKABLE void showMixer();
    Q_INVOKABLE void showFxModule();
    Q_INVOKABLE void showInfo();
    Q_INVOKABLE void showSettings();
    Q_INVOKABLE void showPower();

private:
    void onSubscribe() override;

    void switchRootViewMode(runtime::root_view_mode);
};

} // namespace piejam::gui::model
