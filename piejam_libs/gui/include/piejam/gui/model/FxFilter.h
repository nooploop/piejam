// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxFilter final : public Subscribable<FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::SpectrumData* dataIn READ dataIn CONSTANT)
    Q_PROPERTY(piejam::gui::model::SpectrumData* dataOut READ dataOut CONSTANT)
    Q_PROPERTY(bool spectogramActive READ spectogramActive NOTIFY
                       spectogramActiveChanged FINAL)
    Q_PROPERTY(piejam::gui::model::FxParameter* filterType READ filterType
                       CONSTANT)
    Q_PROPERTY(piejam::gui::model::FxParameter* cutoff READ cutoff CONSTANT)
    Q_PROPERTY(
            piejam::gui::model::FxParameter* resonance READ resonance CONSTANT)

public:
    FxFilter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxFilter();

    auto type() const noexcept -> Type override final { return Type::Filter; }

    bool spectogramActive() const noexcept;

    Q_INVOKABLE void changeSpectogramActive(bool active);

    auto dataIn() noexcept -> SpectrumData*;
    auto dataOut() noexcept -> SpectrumData*;

    Q_INVOKABLE void clear();

    auto filterType() -> FxParameter*;
    auto cutoff() -> FxParameter*;
    auto resonance() -> FxParameter*;

signals:
    void spectogramActiveChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
