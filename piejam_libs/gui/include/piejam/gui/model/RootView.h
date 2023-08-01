// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/root_view_mode.h>

#include <memory>

namespace piejam::gui::model
{

class RootView final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(int mode READ mode NOTIFY modeChanged FINAL)

public:
    RootView(runtime::store_dispatch, runtime::subscriber&);
    ~RootView() override;

    auto mode() -> int
    {
        return m_mode;
    }

    void setMode(int x)
    {
        if (m_mode != x)
        {
            m_mode = x;
            emit modeChanged();
        }
    }

    Q_INVOKABLE void showMixer();
    Q_INVOKABLE void showInfo();
    Q_INVOKABLE void showSettings();
    Q_INVOKABLE void showPower();

signals:
    void modeChanged();

private:
    void onSubscribe() override;

    void switchRootViewMode(runtime::root_view_mode);

    int m_mode{};
};

} // namespace piejam::gui::model
