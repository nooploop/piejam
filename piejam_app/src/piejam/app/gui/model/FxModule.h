// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/box.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::app::gui::model
{

class FxModule final : public Subscribable<piejam::gui::model::FxModule>
{
public:
    FxModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);

    void toggleBypass() override;
    void deleteModule() override;
    void moveLeft() override;
    void moveRight() override;

private:
    void onSubscribe() override;

    runtime::fx::module_id const m_fx_mod_id;

    box<runtime::fx::module_parameters> m_param_ids;
};

} // namespace piejam::app::gui::model
