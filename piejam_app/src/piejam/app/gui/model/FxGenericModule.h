// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::app::gui::model
{

class FxGenericModule final
    : public Subscribable<piejam::gui::model::FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* parameters READ parameters CONSTANT FINAL)

public:
    FxGenericModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxGenericModule();

    auto type() const noexcept -> Type override { return Type::Generic; }

    auto parameters() noexcept -> piejam::gui::model::FxParametersList*;
    auto parameters() const noexcept
            -> piejam::gui::model::FxParametersList const*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::app::gui::model
