// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::fx_modules::filter::gui
{

class FxFilter final
    : public piejam::gui::model::Subscribable<piejam::gui::model::FxModule>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::SpectrumData* dataIn READ dataIn CONSTANT)
    Q_PROPERTY(piejam::gui::model::SpectrumData* dataOut READ dataOut CONSTANT)

    Q_PROPERTY(piejam::gui::model::EnumParameter* filterType READ filterType
                       CONSTANT)
    Q_PROPERTY(piejam::gui::model::FloatParameter* cutoff READ cutoff CONSTANT)
    Q_PROPERTY(piejam::gui::model::FloatParameter* resonance READ resonance
                       CONSTANT)

public:
    FxFilter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxFilter();

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    auto dataIn() noexcept -> piejam::gui::model::SpectrumData*;
    auto dataOut() noexcept -> piejam::gui::model::SpectrumData*;

    Q_INVOKABLE void clear();

    auto filterType() -> piejam::gui::model::EnumParameter*;
    auto cutoff() -> piejam::gui::model::FloatParameter*;
    auto resonance() -> piejam::gui::model::FloatParameter*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::fx_modules::filter::gui
