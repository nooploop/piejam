// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/GenericListModel.h>

#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxGenericModule : public FxModule
{
    Q_OBJECT

    M_PIEJAM_GUI_READONLY_PROPERTY(QAbstractListModel*, parametersList)

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

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
