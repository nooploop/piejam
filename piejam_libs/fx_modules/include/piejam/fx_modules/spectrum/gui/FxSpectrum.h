// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::fx_modules::spectrum::gui
{

class FxSpectrum final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    Q_PROPERTY(
            piejam::gui::model::SpectrumData* dataA READ dataA CONSTANT FINAL)
    Q_PROPERTY(
            piejam::gui::model::SpectrumData* dataB READ dataB CONSTANT FINAL)
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

public:
    FxSpectrum(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxSpectrum() override;

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    auto activeA() const noexcept -> piejam::gui::model::BoolParameter*;
    auto activeB() const noexcept -> piejam::gui::model::BoolParameter*;
    auto channelA() const noexcept -> piejam::gui::model::EnumParameter*;
    auto channelB() const noexcept -> piejam::gui::model::EnumParameter*;
    auto gainA() const noexcept -> piejam::gui::model::FloatParameter*;
    auto gainB() const noexcept -> piejam::gui::model::FloatParameter*;
    auto freeze() const noexcept -> piejam::gui::model::BoolParameter*;

    auto dataA() noexcept -> piejam::gui::model::SpectrumData*
    {
        return &m_spectrumDataA;
    }

    auto dataB() noexcept -> piejam::gui::model::SpectrumData*
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

    piejam::gui::model::SpectrumData m_spectrumDataA;
    piejam::gui::model::SpectrumData m_spectrumDataB;
};

} // namespace piejam::fx_modules::spectrum::gui
