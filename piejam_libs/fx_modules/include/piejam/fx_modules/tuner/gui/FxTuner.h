// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::fx_modules::tuner::gui
{

class FxTuner final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(float, detectedFrequency, setDetectedFrequency)
    M_PIEJAM_GUI_PROPERTY(QString, detectedPitch, setDetectedPitch)
    M_PIEJAM_GUI_PROPERTY(int, detectedCents, setDetectedCents)

public:
    FxTuner(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

private:
    void onSubscribe() override;

    void onChannelChanged();

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::fx_modules::tuner::gui
