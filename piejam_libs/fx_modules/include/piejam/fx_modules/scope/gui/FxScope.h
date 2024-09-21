// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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

class FxScope final
    : public piejam::gui::model::Subscribable<piejam::gui::model::FxModule>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)

    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(piejam::gui::model::EnumParameter* mode READ mode CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::EnumParameter* triggerSlope READ triggerSlope
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FloatParameter* triggerLevel READ
                       triggerLevel CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FloatParameter* holdTime READ holdTime
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::IntParameter* waveformWindowSize READ
                       waveformWindowSize CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::IntParameter* scopeWindowSize READ
                       scopeWindowSize CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::BoolParameter* activeA READ activeA CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::BoolParameter* activeB READ activeB CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::EnumParameter* channelA READ channelA
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::EnumParameter* channelB READ channelB
                       CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::FloatParameter* gainA READ gainA CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::FloatParameter* gainB READ gainB CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::BoolParameter* freeze READ freeze CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataA READ
                       waveformDataA CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataB READ
                       waveformDataB CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeData* scopeDataA READ scopeDataA
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeData* scopeDataB READ scopeDataB
                       CONSTANT FINAL)

public:
    FxScope(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxScope() override;

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    auto busType() const noexcept -> piejam::gui::model::BusType;

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

    auto mode() const noexcept -> piejam::gui::model::EnumParameter*;
    auto triggerSlope() const noexcept -> piejam::gui::model::EnumParameter*;
    auto triggerLevel() const noexcept -> piejam::gui::model::FloatParameter*;
    auto holdTime() const noexcept -> piejam::gui::model::FloatParameter*;
    auto
    waveformWindowSize() const noexcept -> piejam::gui::model::IntParameter*;
    auto scopeWindowSize() const noexcept -> piejam::gui::model::IntParameter*;
    auto activeA() const noexcept -> piejam::gui::model::BoolParameter*;
    auto activeB() const noexcept -> piejam::gui::model::BoolParameter*;
    auto channelA() const noexcept -> piejam::gui::model::EnumParameter*;
    auto channelB() const noexcept -> piejam::gui::model::EnumParameter*;
    auto gainA() const noexcept -> piejam::gui::model::FloatParameter*;
    auto gainB() const noexcept -> piejam::gui::model::FloatParameter*;
    auto freeze() const noexcept -> piejam::gui::model::BoolParameter*;

    auto waveformDataA() noexcept -> piejam::gui::model::WaveformDataObject*
    {
        return &m_waveformDataA;
    }

    auto scopeDataA() noexcept -> piejam::gui::model::ScopeData*
    {
        return &m_scopeDataA;
    }

    auto waveformDataB() noexcept -> piejam::gui::model::WaveformDataObject*
    {
        return &m_waveformDataB;
    }

    auto scopeDataB() noexcept -> piejam::gui::model::ScopeData*
    {
        return &m_scopeDataB;
    }

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
    piejam::gui::model::WaveformDataObject m_waveformDataA;
    piejam::gui::model::WaveformDataObject m_waveformDataB;
    piejam::gui::model::ScopeData m_scopeDataA;
    piejam::gui::model::ScopeData m_scopeDataB;
};

} // namespace piejam::fx_modules::scope::gui
