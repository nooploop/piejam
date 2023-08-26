// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/FxModuleContent.h>
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

class FxScope final : public Subscribable<FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)
    Q_PROPERTY(int samplesPerPixel READ samplesPerPixel WRITE setSamplesPerPixel
                       NOTIFY samplesPerPixelChanged FINAL)
    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(piejam::gui::model::FxEnumParameter* triggerSource READ
                       triggerSource CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxEnumParameter* triggerSlope READ
                       triggerSlope CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FxFloatParameter* triggerLevel READ
                       triggerLevel CONSTANT FINAL)
    Q_PROPERTY(int holdTime READ holdTime WRITE setHoldTime NOTIFY
                       holdTimeChanged FINAL)
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
    Q_PROPERTY(int scopeResolution READ scopeResolution WRITE setScopeResolution
                       NOTIFY scopeResolutionChanged FINAL)
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

    auto samplesPerPixel() const noexcept -> int
    {
        return m_samplesPerPixel;
    }

    void setSamplesPerPixel(int x);

    auto viewSize() const noexcept -> int
    {
        return m_viewSize;
    }

    void setViewSize(int const x);

    enum class TriggerSource
    {
        Free,
        StreamA,
        StreamB,
    };

    Q_ENUM(TriggerSource)

    auto triggerSource() const noexcept -> FxEnumParameter*;

    auto triggerSlope() const noexcept -> FxEnumParameter*;

    auto triggerLevel() const noexcept -> FxFloatParameter*;

    auto holdTime() const noexcept -> int
    {
        return m_holdTime;
    }

    void setHoldTime(int holdTimeInMS);

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

    auto scopeResolution() const noexcept -> int
    {
        return m_scopeResolution;
    }

    void setScopeResolution(int);

    auto freeze() const noexcept -> bool
    {
        return m_freeze;
    }

    void setFreeze(bool);

    Q_INVOKABLE void clear();

signals:
    void samplesPerPixelChanged();
    void viewSizeChanged();
    void holdTimeChanged();
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

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_samplesPerPixel{1};
    int m_viewSize{};
    int m_holdTime{80};
    WaveformDataObject m_waveformDataA;
    WaveformDataObject m_waveformDataB;
    ScopeData m_scopeDataA;
    ScopeData m_scopeDataB;
    bool m_activeA{true};
    bool m_activeB{false};
    StereoChannel m_channelA{StereoChannel::Left};
    StereoChannel m_channelB{StereoChannel::Right};
    int m_scopeResolution{1};
    bool m_freeze{};
};

} // namespace piejam::gui::model
