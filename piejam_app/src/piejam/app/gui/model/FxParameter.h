// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameters.h>

namespace piejam::app::gui::model
{

class FxParameter final : public Subscribable<piejam::gui::model::FxParameter>
{
public:
    FxParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::parameter_id);
    ~FxParameter();

    void changeValue(double) override;
    void changeSwitchValue(bool) override;

    auto midi() const -> piejam::gui::model::MidiAssignable* override;

private:
    void onSubscribe() override;

    runtime::fx::parameter_id m_fx_param_id;

    std::unique_ptr<piejam::gui::model::MidiAssignable> m_midi;
};

} // namespace piejam::app::gui::model
