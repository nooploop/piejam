// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxIntParameter.h>

#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct FxIntParameter::Impl
{
    runtime::int_parameter_id param_id;
};

FxIntParameter::FxIntParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxParameterKeyId const& param)
    : FxParameter{store_dispatch, state_change_subscriber, param}
    , m_impl{std::make_unique<Impl>(
              std::get<runtime::int_parameter_id>(param.id))}

{
    setMinValue(
            observe_once(runtime::selectors::make_int_parameter_min_selector(
                    m_impl->param_id)));

    setMaxValue(
            observe_once(runtime::selectors::make_int_parameter_max_selector(
                    m_impl->param_id)));
}

FxIntParameter::~FxIntParameter() = default;

void
FxIntParameter::onSubscribe()
{
    FxParameter::onSubscribe();

    observe(runtime::selectors::make_int_parameter_value_selector(
                    m_impl->param_id),
            [this](int const value) { setValue(value); });
}

void
FxIntParameter::changeValue(int value)
{
    dispatch(runtime::actions::set_int_parameter(m_impl->param_id, value));
}

} // namespace piejam::gui::model
