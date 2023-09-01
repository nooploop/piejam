// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/EnumListModel.h>
#include <piejam/gui/model/FxIntParameter.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxEnumParameter final : public FxIntParameter
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* values READ values CONSTANT FINAL)

public:
    FxEnumParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::FxParameterId const&);
    ~FxEnumParameter();

    static inline constexpr auto StaticType = Type::Enum;

    auto type() const noexcept -> Type override
    {
        return FxEnumParameter::StaticType;
    }

    auto values() const noexcept -> EnumListModel*;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
