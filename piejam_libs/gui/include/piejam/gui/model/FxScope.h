// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/ScopeData.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/WaveformDataObject.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class FxScope final : public Subscribable<FxModuleContent>
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

    auto type() const noexcept -> Type override
    {
        return Type::Scope;
    }

    auto busType() const noexcept -> BusType;

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

    auto mode() const noexcept -> EnumParameter*;
    auto triggerSlope() const noexcept -> EnumParameter*;
    auto triggerLevel() const noexcept -> FloatParameter*;
    auto holdTime() const noexcept -> FloatParameter*;
    auto waveformWindowSize() const noexcept -> IntParameter*;
    auto scopeWindowSize() const noexcept -> IntParameter*;
    auto activeA() const noexcept -> BoolParameter*;
    auto activeB() const noexcept -> BoolParameter*;
    auto channelA() const noexcept -> EnumParameter*;
    auto channelB() const noexcept -> EnumParameter*;
    auto gainA() const noexcept -> FloatParameter*;
    auto gainB() const noexcept -> FloatParameter*;
    auto freeze() const noexcept -> BoolParameter*;

    auto waveformDataA() noexcept -> WaveformDataObject*
    {
        return &m_waveformDataA;
    }

    auto scopeDataA() noexcept -> ScopeData*
    {
        return &m_scopeDataA;
    }

    auto waveformDataB() noexcept -> WaveformDataObject*
    {
        return &m_waveformDataB;
    }

    auto scopeDataB() noexcept -> ScopeData*
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
    WaveformDataObject m_waveformDataA;
    WaveformDataObject m_waveformDataB;
    ScopeData m_scopeDataA;
    ScopeData m_scopeDataB;
};

} // namespace piejam::gui::model
