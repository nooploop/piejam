// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/ScopeSlot.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/WaveformSlot.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::fx_modules::scope::gui
{

class FxScope final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    M_PIEJAM_GUI_WRITABLE_PROPERTY(int, viewSize, setViewSize)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::EnumParameter*, mode)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::EnumParameter*,
            triggerSlope)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::FloatParameter*,
            triggerLevel)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::FloatParameter*,
            holdTime)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::IntParameter*,
            waveformWindowSize)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::IntParameter*,
            scopeWindowSize)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, activeA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, activeB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::EnumParameter*, channelA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::EnumParameter*, channelB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::FloatParameter*, gainA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::FloatParameter*, gainB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, freeze)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::WaveformSlot*, waveformA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::WaveformSlot*, waveformB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::ScopeSlot*, scopeA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::ScopeSlot*, scopeB)

public:
    FxScope(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    enum class Mode
    {
        Free,
        TriggerA,
        TriggerB,
    };

    Q_ENUM(Mode)

    Q_INVOKABLE void clear();

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::fx_modules::scope::gui
