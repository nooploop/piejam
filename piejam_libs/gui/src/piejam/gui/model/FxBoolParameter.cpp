// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxBoolParameter.h>

#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct FxBoolParameter::Impl
{
    runtime::bool_parameter_id param_id;
};

FxBoolParameter::FxBoolParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxParameterId const& param)
    : FxParameter(store_dispatch, state_change_subscriber, param)
    , m_impl{std::make_unique<Impl>(
              std::get<runtime::bool_parameter_id>(param))}
{
}

FxBoolParameter::~FxBoolParameter() = default;

void
FxBoolParameter::onSubscribe()
{
    FxParameter::onSubscribe();

    observe(runtime::selectors::make_bool_parameter_value_selector(
                    m_impl->param_id),
            [this](bool const value) { setValue(value); });
}

void
FxBoolParameter::changeValue(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_impl->param_id, value));
}

} // namespace piejam::gui::model
