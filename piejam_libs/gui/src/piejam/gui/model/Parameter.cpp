// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Parameter.h>

#include <piejam/gui/model/BoolParameter.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/IntParameter.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/integral_constant.h>
#include <piejam/math.h>
#include <piejam/runtime/actions/reset_fx_parameter_to_default_value.h>
#include <piejam/runtime/actions/set_float_parameter_normalized.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <boost/mp11/map.hpp>

#include <fmt/format.h>

namespace piejam::gui::model
{

namespace
{

using parameter_id_to_FxParameter = boost::mp11::mp_list<
        boost::mp11::mp_list<runtime::bool_parameter_id, BoolParameter>,
        boost::mp11::mp_list<runtime::float_parameter_id, FloatParameter>,
        boost::mp11::mp_list<runtime::int_parameter_id, IntParameter>>;

} // namespace

struct Parameter::Impl
{
    ParameterId param;
    std::unique_ptr<MidiAssignable> midi;
};

Parameter::Parameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        ParameterId const& param)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              param,
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      param)))
{
    setName(QString::fromStdString(observe_once(
            runtime::selectors::make_fx_parameter_name_selector(param))));

    setBipolar(observe_once(
            runtime::selectors::make_fx_parameter_bipolar_selector(param)));
}

Parameter::~Parameter() = default;

auto
Parameter::type() const noexcept -> Type
{
    return std::visit(
            []<class T>(T const&) -> Type {
                return boost::mp11::mp_at_c<
                        boost::mp11::
                                mp_map_find<parameter_id_to_FxParameter, T>,
                        1>::StaticType;
            },
            m_impl->param);
}

void
Parameter::onSubscribe()
{
    observe(runtime::selectors::make_fx_parameter_value_string_selector(
                    m_impl->param),
            [this](std::string const& text) {
                setValueString(QString::fromStdString(text));
            });
}

auto
Parameter::midi() const -> MidiAssignable*
{
    return m_impl->midi.get();
}

void
Parameter::resetToDefault()
{
    dispatch(runtime::actions::reset_fx_parameter_to_default_value(
            m_impl->param));
}

auto
Parameter::paramId() const -> ParameterId
{
    return m_impl->param;
}

auto
makeParameter(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        piejam::gui::model::ParameterId const& paramId)
        -> std::unique_ptr<Parameter>
{
    return std::visit(
            [&]<class T>(T const&) -> std::unique_ptr<Parameter> {
                using FxParameterType = boost::mp11::mp_at_c<
                        boost::mp11::
                                mp_map_find<parameter_id_to_FxParameter, T>,
                        1>;
                return std::make_unique<FxParameterType>(
                        dispatch,
                        state_change_subscriber,
                        paramId);
            },
            paramId);
}

} // namespace piejam::gui::model
