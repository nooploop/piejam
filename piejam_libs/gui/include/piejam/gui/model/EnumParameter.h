// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/EnumListModel.h>
#include <piejam/gui/model/IntParameter.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class EnumParameter final : public IntParameter
{
    Q_OBJECT

    M_PIEJAM_GUI_CONSTANT_PROPERTY(QAbstractListModel*, values)

public:
    EnumParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::parameter_id);

    static constexpr auto StaticType = Type::Enum;

    auto type() const noexcept -> Type override
    {
        return EnumParameter::StaticType;
    }

    template <class E>
        requires(std::is_enum_v<E> &&
                 std::is_same_v<std::underlying_type_t<E>, int>)
    auto as() const noexcept -> E
    {
        return static_cast<E>(value());
    }

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
