// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <piejam/app/gui/model/FxParameter.h>

#include <piejam/math.h>
#include <piejam/runtime/actions/set_float_parameter_normalized.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::app::gui::model
{

FxParameter::FxParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::parameter_id fx_param_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_fx_param_id(fx_param_id)
{
    setStepped(
            std::holds_alternative<runtime::int_parameter_id>(m_fx_param_id));
    setIsSwitch(
            std::holds_alternative<runtime::bool_parameter_id>(m_fx_param_id));
}

void
FxParameter::subscribe_step()
{
    observe(runtime::selectors::make_fx_parameter_name_selector(m_fx_param_id),
            [this](container::boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    if (auto id = std::get_if<runtime::float_parameter_id>(&m_fx_param_id))
    {
        observe(runtime::selectors::
                        make_float_parameter_normalized_value_selector(*id),
                [this](float const value) { setValue(value); });

        observe(runtime::selectors::make_float_parameter_value_selector(*id),
                [this](float const value) {
                    m_value = value;
                    updateValueString();
                });
    }
    else if (auto id = std::get_if<runtime::int_parameter_id>(&m_fx_param_id))
    {
        observe(runtime::selectors::make_int_parameter_min_selector(*id),
                [this](int const min) { setMinValue(min); });

        observe(runtime::selectors::make_int_parameter_max_selector(*id),
                [this](int const max) { setMaxValue(max); });

        observe(runtime::selectors::make_int_parameter_value_selector(*id),
                [this](int const value) { setValue(value); });
    }
    else if (auto id = std::get_if<runtime::bool_parameter_id>(&m_fx_param_id))
    {
        observe(runtime::selectors::make_bool_parameter_value_selector(*id),
                [this](bool const value) { setSwitchValue(value); });
    }

    observe(runtime::selectors::make_fx_parameter_unit_selector(m_fx_param_id),
            [this](runtime::fx::parameter_unit const unit) {
                m_unit = unit;
                updateValueString();
            });
}

void
FxParameter::changeValue(double value)
{
    BOOST_ASSERT(
            std::holds_alternative<runtime::float_parameter_id>(
                    m_fx_param_id) ||
            std::holds_alternative<runtime::int_parameter_id>(m_fx_param_id));
    if (auto id = std::get_if<runtime::float_parameter_id>(&m_fx_param_id))
    {
        dispatch(runtime::actions::set_float_parameter_normalized(*id, value));
    }
    else if (auto id = std::get_if<runtime::int_parameter_id>(&m_fx_param_id))
    {
        dispatch(runtime::actions::set_int_parameter(
                *id,
                static_cast<int>(value)));
    }
}

void
FxParameter::changeSwitchValue(bool value)
{
    BOOST_ASSERT(
            std::holds_alternative<runtime::bool_parameter_id>(m_fx_param_id));
    if (auto id = std::get_if<runtime::bool_parameter_id>(&m_fx_param_id))
    {
        dispatch(runtime::actions::set_bool_parameter(*id, value));
    }
}

void
FxParameter::updateValueString()
{
    switch (m_unit)
    {
        case runtime::fx::parameter_unit::none:
            return setValueString(
                    QString::fromStdString(fmt::format("{:.2f}", m_value)));

        case runtime::fx::parameter_unit::dB:
            if (m_value == 0.f)
                return setValueString("-Inf");
            else
                return setValueString(QString::fromStdString(
                        fmt::format("{:+.1f} dB", math::to_dB(m_value))));
    }
}

} // namespace piejam::app::gui::model
