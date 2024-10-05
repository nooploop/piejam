// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::gui::model
{

class StereoLevelParameter : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, levelLeft, setLevelLeft)
    M_PIEJAM_GUI_PROPERTY(double, levelRight, setLevelRight)

public:
    StereoLevelParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::stereo_level_parameter_id const&);
    ~StereoLevelParameter() override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
