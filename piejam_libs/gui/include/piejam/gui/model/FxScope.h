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
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataA READ
                       waveformDataA CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataB READ
                       waveformDataB CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeData* scopeDataA READ scopeDataA
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeData* scopeDataB READ scopeDataB
                       CONSTANT FINAL)
    Q_PROPERTY(bool activeA READ activeA NOTIFY activeAChanged FINAL)
    Q_PROPERTY(bool activeB READ activeB NOTIFY activeBChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelA READ channelA NOTIFY
                       channelAChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelB READ channelB NOTIFY
                       channelBChanged FINAL)
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

    auto activeA() const noexcept -> bool
    {
        return m_activeA;
    }

    Q_INVOKABLE void changeActiveA(bool active);

    auto channelA() const noexcept -> StereoChannel
    {
        return m_channelA;
    }

    Q_INVOKABLE void changeChannelA(piejam::gui::model::StereoChannel);

    auto waveformDataA() noexcept -> WaveformDataObject*
    {
        return &m_waveformDataA;
    }

    auto scopeDataA() noexcept -> ScopeData*
    {
        return &m_scopeDataA;
    }

    auto activeB() const noexcept -> bool
    {
        return m_activeB;
    }

    Q_INVOKABLE void changeActiveB(bool active);

    auto channelB() const noexcept -> StereoChannel
    {
        return m_channelB;
    }

    Q_INVOKABLE void changeChannelB(piejam::gui::model::StereoChannel);

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
    void samplesPerPixelChanged();
    void viewSizeChanged();
    void activeAChanged();
    void activeBChanged();
    void channelAChanged();
    void channelBChanged();
    void scopeResolutionChanged();
    void freezeChanged();

private:
    void onSubscribe() override;

    void onTriggerSourceChanged();
    void onTriggerSlopeChanged();
    void onTriggerLevelChanged();
    void onHoldTimeChanged();
    void onWaveformWindowSizeChanged();
    void onScopeWindowSizeChanged();

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_viewSize{};
    WaveformDataObject m_waveformDataA;
    WaveformDataObject m_waveformDataB;
    ScopeData m_scopeDataA;
    ScopeData m_scopeDataB;
    bool m_activeA{true};
    bool m_activeB{false};
    StereoChannel m_channelA{StereoChannel::Left};
    StereoChannel m_channelB{StereoChannel::Right};
    bool m_freeze{};
};

} // namespace piejam::gui::model
