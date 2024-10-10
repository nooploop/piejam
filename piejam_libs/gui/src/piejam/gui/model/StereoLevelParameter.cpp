// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/StereoLevelParameter.h>

#include <piejam/runtime/actions/audio_engine_sync.h>
#include <piejam/runtime/selectors.h>

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
        runtime::parameter_id param_id)
    : Parameter{store_dispatch, state_change_subscriber, param_id}
    , m_impl{make_pimpl<Impl>(
              std::get<runtime::stereo_level_parameter_id>(param_id))}
{
}

void
StereoLevelParameter::onSubscribe()
{
    observe(runtime::selectors::make_level_parameter_value_selector(
                    m_impl->param_id),
            [this](runtime::stereo_level const& x) {
                setLevelLeft(x.left);
                setLevelRight(x.right);
            });

    runtime::actions::sync_parameter action;
    action.param = this->paramId();
    dispatch(action);
}

void
StereoLevelParameter::onUnsubscribe()
{
    runtime::actions::unsync_parameter action;
    action.param = this->paramId();
    dispatch(action);
}

} // namespace piejam::gui::model
