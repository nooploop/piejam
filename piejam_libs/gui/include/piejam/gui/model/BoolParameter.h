// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Parameter.h>

#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class BoolParameter final : public Parameter
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(bool, value, setValue)

public:
    BoolParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::parameter_id);

    static constexpr auto StaticType = Type::Bool;

    Q_INVOKABLE void changeValue(bool);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
