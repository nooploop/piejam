// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/FxModuleContentSubscribable.h>
#include <piejam/gui/model/ScopeData.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/WaveformDataObject.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class FxScope final : public FxModuleContentSubscribable
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)

    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(
            piejam::gui::model::FxEnumParameter* mode READ mode CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxEnumParameter* triggerSlope READ
                       triggerSlope CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxFloatParameter* triggerLevel READ
                       triggerLevel CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxFloatParameter* holdTime READ holdTime
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxIntParameter* waveformWindowSize READ
                       waveformWindowSize CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxIntParameter* scopeWindowSize READ
                       scopeWindowSize CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxBoolParameter* activeA READ activeA
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxBoolParameter* activeB READ activeB
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxEnumParameter* channelA READ channelA
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxEnumParameter* channelB READ channelB
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxFloatParameter* gainA READ gainA CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::FxFloatParameter* gainB READ gainB CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataA READ
                       waveformDataA CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataB READ
                       waveformDataB CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeData* scopeDataA READ scopeDataA
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeData* scopeDataB READ scopeDataB
                       CONSTANT FINAL)
    Q_PROPERTY(
            bool freeze READ freeze WRITE setFreeze NOTIFY freezeChanged FINAL)

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

    auto mode() const noexcept -> FxEnumParameter*;
    auto triggerSlope() const noexcept -> FxEnumParameter*;
    auto triggerLevel() const noexcept -> FxFloatParameter*;
    auto holdTime() const noexcept -> FxFloatParameter*;
    auto waveformWindowSize() const noexcept -> FxIntParameter*;
    auto scopeWindowSize() const noexcept -> FxIntParameter*;
    auto activeA() const noexcept -> FxBoolParameter*;
    auto activeB() const noexcept -> FxBoolParameter*;
    auto channelA() const noexcept -> FxEnumParameter*;
    auto channelB() const noexcept -> FxEnumParameter*;
    auto gainA() const noexcept -> FxFloatParameter*;
    auto gainB() const noexcept -> FxFloatParameter*;

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

    auto freeze() const noexcept -> bool
    {
        return m_freeze;
    }

    void setFreeze(bool);

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

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_viewSize{};
    WaveformDataObject m_waveformDataA;
    WaveformDataObject m_waveformDataB;
    ScopeData m_scopeDataA;
    ScopeData m_scopeDataB;
    bool m_freeze{};
};

} // namespace piejam::gui::model
