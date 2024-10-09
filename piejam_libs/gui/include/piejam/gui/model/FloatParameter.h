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

class FloatParameter : public Parameter
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, value, setValue)
    M_PIEJAM_GUI_PROPERTY(double, normalizedValue, setNormalizedValue)
    M_PIEJAM_GUI_PROPERTY(bool, bipolar, setBipolar)

public:
    FloatParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::parameter_id);

    static constexpr auto StaticType = Type::Float;

    Q_INVOKABLE void changeValue(double);
    Q_INVOKABLE void changeNormalizedValue(double);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
