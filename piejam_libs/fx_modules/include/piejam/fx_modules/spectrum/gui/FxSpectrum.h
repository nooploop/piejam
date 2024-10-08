// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::fx_modules::spectrum::gui
{

class FxSpectrum final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::SpectrumData*, dataA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::SpectrumData*, dataB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, activeA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, activeB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::EnumParameter*, channelA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::EnumParameter*, channelB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::FloatParameter*, gainA)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::FloatParameter*, gainB)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, freeze)

public:
    FxSpectrum(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    Q_INVOKABLE void clear();

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
    pimpl<Impl> m_impl;
};

} // namespace piejam::fx_modules::spectrum::gui
