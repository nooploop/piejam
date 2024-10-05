// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxModuleView : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(MaterialColor, color, setColor)
    M_PIEJAM_GUI_PROPERTY(QString, chainName, setChainName)
    M_PIEJAM_GUI_PROPERTY(QString, name, setName)
    M_PIEJAM_GUI_PROPERTY(bool, bypassed, setBypassed)

    Q_PROPERTY(piejam::gui::model::FxModule* content READ content NOTIFY
                       contentChanged FINAL)

public:
    FxModuleView(runtime::store_dispatch, runtime::subscriber&);
    ~FxModuleView();

    auto content() noexcept -> FxModule*;

    Q_INVOKABLE void toggleBypass();

signals:
    void contentChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
