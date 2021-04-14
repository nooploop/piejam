// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxScopeModule final
    : public Subscribable<piejam::gui::model::FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::AudioStreamProvider* scopeStream READ
                       scopeStream CONSTANT FINAL)

public:
    FxScopeModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxScopeModule();

    auto type() const noexcept -> Type override { return Type::Scope; }

    auto scopeStream() noexcept -> piejam::gui::model::AudioStreamProvider*;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
