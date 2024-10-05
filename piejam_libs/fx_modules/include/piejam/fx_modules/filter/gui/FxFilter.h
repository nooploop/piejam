// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::fx_modules::filter::gui
{

class FxFilter final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::SpectrumData*, dataIn)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::SpectrumData*, dataOut)

    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::EnumParameter*,
            filterType)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::FloatParameter*, cutoff)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::FloatParameter*,
            resonance)

public:
    FxFilter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxFilter();

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    Q_INVOKABLE void clear();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::fx_modules::filter::gui
