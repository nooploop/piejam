// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxScope.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxScope final : public Subscribable<piejam::gui::model::FxScope>
{
public:
    FxScope(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxScope();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
