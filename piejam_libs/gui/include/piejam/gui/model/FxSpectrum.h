// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContentSubscribable.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxSpectrum final : public FxModuleContentSubscribable
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::SpectrumData* dataA READ dataA CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::SpectrumData* dataB READ dataB CONSTANT FINAL)
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
    Q_PROPERTY(piejam::gui::model::FxBoolParameter* freeze READ freeze CONSTANT
                       FINAL)

public:
    FxSpectrum(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxSpectrum() override;

    auto type() const noexcept -> Type override
    {
        return Type::Spectrum;
    }

    auto busType() const noexcept -> BusType;

    auto activeA() const noexcept -> FxBoolParameter*;
    auto activeB() const noexcept -> FxBoolParameter*;
    auto channelA() const noexcept -> FxEnumParameter*;
    auto channelB() const noexcept -> FxEnumParameter*;
    auto gainA() const noexcept -> FxFloatParameter*;
    auto gainB() const noexcept -> FxFloatParameter*;
    auto freeze() const noexcept -> FxBoolParameter*;

    auto dataA() noexcept -> SpectrumData*
    {
        return &m_spectrumDataA;
    }

    auto dataB() noexcept -> SpectrumData*
    {
        return &m_spectrumDataB;
    }

    Q_INVOKABLE void clear()
    {
        m_spectrumDataA.clear();
        m_spectrumDataB.clear();
    }

private:
    void onSubscribe() override;

    void onActiveAChanged();
    void onActiveBChanged();
    void onChannelAChanged();
    void onChannelBChanged();
    void onGainAChanged();
    void onGainBChanged();
    void onFreezeChanged();

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    SpectrumData m_spectrumDataA;
    SpectrumData m_spectrumDataB;
};

} // namespace piejam::gui::model
