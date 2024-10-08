// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/BoolParameter.h>

#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct BoolParameter::Impl
{
    runtime::bool_parameter_id param_id;
};

BoolParameter::BoolParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        ParameterId const& param)
    : Parameter(store_dispatch, state_change_subscriber, param)
    , m_impl{make_pimpl<Impl>(std::get<runtime::bool_parameter_id>(param))}
{
}

void
BoolParameter::onSubscribe()
{
    Parameter::onSubscribe();

    observe(runtime::selectors::make_bool_parameter_value_selector(
                    m_impl->param_id),
            [this](bool const value) { setValue(value); });
}

void
BoolParameter::changeValue(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_impl->param_id, value));
}

} // namespace piejam::gui::model
