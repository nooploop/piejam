// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxParameter.h>

#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/math.h>
#include <piejam/runtime/actions/set_float_parameter_normalized.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct FxParameter::Impl
{
    FxParameterKeyId param;
    std::unique_ptr<MidiAssignable> midi;
};

FxParameter::FxParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxParameterKeyId const& param)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              param,
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      param.id)))
{
    setStepped(std::holds_alternative<runtime::int_parameter_id>(
            m_impl->param.id));
    setIsSwitch(std::holds_alternative<runtime::bool_parameter_id>(
            m_impl->param.id));
}

FxParameter::~FxParameter() = default;

void
FxParameter::onSubscribe()
{
    observe(runtime::selectors::make_fx_parameter_name_selector(
                    m_impl->param.id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    if (auto id = std::get_if<runtime::float_parameter_id>(&m_impl->param.id))
    {
        observe(runtime::selectors::
                        make_float_parameter_normalized_value_selector(*id),
                [this](float const value) { setValue(value); });
    }
    else if (
            auto id = std::get_if<runtime::int_parameter_id>(&m_impl->param.id))
    {
        observe(runtime::selectors::make_int_parameter_min_selector(*id),
                [this](int const min) { setMinValue(min); });

        observe(runtime::selectors::make_int_parameter_max_selector(*id),
                [this](int const max) { setMaxValue(max); });

        observe(runtime::selectors::make_int_parameter_value_selector(*id),
                [this](int const value) { setValue(value); });
    }
    else if (
            auto id =
                    std::get_if<runtime::bool_parameter_id>(&m_impl->param.id))
    {
        observe(runtime::selectors::make_bool_parameter_value_selector(*id),
                [this](bool const value) { setSwitchValue(value); });
    }

    observe(runtime::selectors::make_fx_parameter_value_string_selector(
                    m_impl->param.id),
            [this](std::string const& text) {
                setValueString(QString::fromStdString(text));
            });
}

void
FxParameter::changeValue(double value)
{
    if (auto id = std::get_if<runtime::float_parameter_id>(&m_impl->param.id))
    {
        dispatch(runtime::actions::set_float_parameter_normalized(*id, value));
    }
    else if (
            auto id = std::get_if<runtime::int_parameter_id>(&m_impl->param.id))
    {
        dispatch(runtime::actions::set_int_parameter(
                *id,
                static_cast<int>(value)));
    }
    else
    {
        BOOST_ASSERT_MSG(false, "not float nor int parameter");
    }
}

void
FxParameter::changeSwitchValue(bool value)
{
    if (auto id = std::get_if<runtime::bool_parameter_id>(&m_impl->param.id))
    {
        dispatch(runtime::actions::set_bool_parameter(*id, value));
        return;
    }
    else
    {
        BOOST_ASSERT_MSG(false, "not a bool parameter");
    }
}

auto
FxParameter::midi() const -> MidiAssignable*
{
    return m_impl->midi.get();
}

} // namespace piejam::gui::model
