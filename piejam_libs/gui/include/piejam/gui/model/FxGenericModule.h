// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxGenericModule : public FxModule
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* parametersList READ parametersList CONSTANT
                       FINAL)

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

    auto parametersList() noexcept -> FxParametersList*;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
