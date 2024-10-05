// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/ScopeData.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/WaveformDataObject.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::fx_modules::scope::gui
{

class FxScope final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
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
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::WaveformDataObject*,
            waveformDataA)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::WaveformDataObject*,
            waveformDataB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::ScopeData*, scopeDataA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::ScopeData*, scopeDataB)

public:
    FxScope(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxScope() override;

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    auto viewSize() const noexcept -> int
    {
        return m_viewSize;
    }

    void setViewSize(int const x);

    enum class Mode
    {
        Free,
        StreamA,
        StreamB,
    };

    Q_ENUM(Mode)

    Q_INVOKABLE void clear();

signals:
    void viewSizeChanged();
    void freezeChanged();

private:
    void onSubscribe() override;

    void onTriggerSourceChanged();
    void onTriggerSlopeChanged();
    void onTriggerLevelChanged();
    void onHoldTimeChanged();
    void onWaveformWindowSizeChanged();
    void onScopeWindowSizeChanged();
    void onActiveAChanged();
    void onActiveBChanged();
    void onChannelAChanged();
    void onChannelBChanged();
    void onGainAChanged();
    void onGainBChanged();
    void onFreezeChanged();

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_viewSize{};
};

} // namespace piejam::fx_modules::scope::gui
