// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Parameter.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/parameters.h>

namespace piejam::gui::model
{

class StereoLevelParameter : public Parameter
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, levelLeft, setLevelLeft)
    M_PIEJAM_GUI_PROPERTY(double, levelRight, setLevelRight)

public:
    StereoLevelParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::parameter_id);

    static constexpr auto StaticType = Type::StereoLevel;

private:
    void onSubscribe() override;
    void onUnsubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
