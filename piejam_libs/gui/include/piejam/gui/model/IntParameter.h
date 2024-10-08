// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Parameter.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

namespace piejam::gui::model
{

class IntParameter : public Parameter
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(int, value, setValue)
    M_PIEJAM_GUI_PROPERTY(int, minValue, setMinValue)
    M_PIEJAM_GUI_PROPERTY(int, maxValue, setMaxValue)

public:
    IntParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            piejam::gui::model::ParameterId const&);

    static constexpr auto StaticType = Type::Int;

    Q_INVOKABLE void changeValue(int);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
