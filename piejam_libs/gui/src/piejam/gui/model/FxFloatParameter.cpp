// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxFloatParameter.h>

#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/runtime/actions/set_float_parameter_normalized.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct FxFloatParameter::Impl
{
    runtime::float_parameter_id param_id;
};

FxFloatParameter::FxFloatParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxParameterId const& param)
    : FxParameter{store_dispatch, state_change_subscriber, param}
    , m_impl{std::make_unique<Impl>(
              std::get<runtime::float_parameter_id>(param))}
{
}

FxFloatParameter::~FxFloatParameter() = default;

void
FxFloatParameter::onSubscribe()
{
    FxParameter::onSubscribe();

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_impl->param_id),
            [this](float const value) { setValue(value); });

    observe(runtime::selectors::make_float_parameter_normalized_value_selector(
                    m_impl->param_id),
            [this](float const value) { setNormalizedValue(value); });
}

void
FxFloatParameter::changeNormalizedValue(double value)
{
    dispatch(runtime::actions::set_float_parameter_normalized(
            m_impl->param_id,
            value));
}

} // namespace piejam::gui::model
