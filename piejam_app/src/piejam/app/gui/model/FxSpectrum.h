// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxSpectrum.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxSpectrum final : public Subscribable<piejam::gui::model::FxSpectrum>
{
public:
    FxSpectrum(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxSpectrum();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
