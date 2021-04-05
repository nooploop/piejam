// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxModule final : public Subscribable<piejam::gui::model::FxModule>
{
public:
    FxModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id,
            runtime::fx::instance_id);
    ~FxModule();

    void toggleBypass() override;
    void deleteModule() override;
    void moveLeft() override;
    void moveRight() override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
