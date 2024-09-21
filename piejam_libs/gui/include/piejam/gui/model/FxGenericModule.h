// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxGenericModule : public Subscribable<FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* parameters READ parameters CONSTANT FINAL)

public:
    FxGenericModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxGenericModule();

    auto type() const noexcept -> FxModuleType override
    {
        return {};
    }

    auto parameters() noexcept -> FxParametersList*;
    auto parameters() const noexcept -> FxParametersList const*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
