// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StereoLevelParameter.h>

#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct StereoLevelParameter::Impl
{
    runtime::stereo_level_parameter_id param_id;
};

StereoLevelParameter::StereoLevelParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::stereo_level_parameter_id const& param_id)
    : Subscribable{store_dispatch, state_change_subscriber}
    , m_impl{std::make_unique<Impl>(param_id)}
{
}

StereoLevelParameter::~StereoLevelParameter() = default;

void
StereoLevelParameter::onSubscribe()
{
    observe(runtime::selectors::make_level_parameter_value_selector(
                    m_impl->param_id),
            [this](runtime::stereo_level const& x) {
                setLevelLeft(x.left);
                setLevelRight(x.right);
            });
}

} // namespace piejam::gui::model
