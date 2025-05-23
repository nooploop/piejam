// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/IntParameter.h>

#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct IntParameter::Impl
{
    runtime::int_parameter_id param_id;
};

IntParameter::IntParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::parameter_id param_id)
    : Parameter{store_dispatch, state_change_subscriber, param_id}
    , m_impl{make_pimpl<Impl>(std::get<runtime::int_parameter_id>(param_id))}

{
    setMinValue(
            observe_once(runtime::selectors::make_int_parameter_min_selector(
                    m_impl->param_id)));

    setMaxValue(
            observe_once(runtime::selectors::make_int_parameter_max_selector(
                    m_impl->param_id)));
}

void
IntParameter::onSubscribe()
{
    Parameter::onSubscribe();

    observe(runtime::selectors::make_int_parameter_value_selector(
                    m_impl->param_id),
            [this](int const value) { setValue(value); });
}

void
IntParameter::changeValue(int value)
{
    dispatch(runtime::actions::set_int_parameter(m_impl->param_id, value));
}

} // namespace piejam::gui::model
