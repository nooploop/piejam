// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxParameter.h>

#include <piejam/app/gui/model/MidiAssignable.h>
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
    , m_midi(std::make_unique<piejam::app::gui::model::MidiAssignable>(
              store_dispatch,
              state_change_subscriber,
              fx_param_id))
{
    setStepped(
            std::holds_alternative<runtime::int_parameter_id>(m_fx_param_id));
    setIsSwitch(
            std::holds_alternative<runtime::bool_parameter_id>(m_fx_param_id));
}

FxParameter::~FxParameter() = default;

void
FxParameter::onSubscribe()
{
    observe(runtime::selectors::make_fx_parameter_name_selector(m_fx_param_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    if (auto id = std::get_if<runtime::float_parameter_id>(&m_fx_param_id))
    {
        observe(runtime::selectors::
                        make_float_parameter_normalized_value_selector(*id),
                [this](float const value) { setValue(value); });
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

    observe(runtime::selectors::make_fx_parameter_value_string_selector(
                    m_fx_param_id),
            [this](std::string const& text) {
                setValueString(QString::fromStdString(text));
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

auto
FxParameter::midi() const -> piejam::gui::model::MidiAssignable*
{
    return m_midi.get();
}

} // namespace piejam::app::gui::model
