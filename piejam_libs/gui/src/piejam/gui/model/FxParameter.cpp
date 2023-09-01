// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxParameter.h>

#include <piejam/gui/model/FxBoolParameter.h>
#include <piejam/gui/model/FxFloatParameter.h>
#include <piejam/gui/model/FxIntParameter.h>
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
        boost::mp11::mp_list<runtime::bool_parameter_id, FxBoolParameter>,
        boost::mp11::mp_list<runtime::float_parameter_id, FxFloatParameter>,
        boost::mp11::mp_list<runtime::int_parameter_id, FxIntParameter>>;

} // namespace

struct FxParameter::Impl
{
    FxParameterId param;
    std::unique_ptr<MidiAssignable> midi;
};

FxParameter::FxParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        FxParameterId const& param)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              param,
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      param)))
{
    setName(QString::fromStdString(*observe_once(
            runtime::selectors::make_fx_parameter_name_selector(param))));
}

FxParameter::~FxParameter() = default;

auto
FxParameter::type() const noexcept -> Type
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
FxParameter::onSubscribe()
{
    observe(runtime::selectors::make_fx_parameter_value_string_selector(
                    m_impl->param),
            [this](std::string const& text) {
                setValueString(QString::fromStdString(text));
            });
}

auto
FxParameter::midi() const -> MidiAssignable*
{
    return m_impl->midi.get();
}

void
FxParameter::resetToDefault()
{
    dispatch(runtime::actions::reset_fx_parameter_to_default_value(
            m_impl->param));
}

auto
FxParameter::paramKeyId() const -> FxParameterId
{
    return m_impl->param;
}

auto
makeFxParameter(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        piejam::gui::model::FxParameterId const& paramId)
        -> std::unique_ptr<FxParameter>
{
    return std::visit(
            [&]<class T>(T const&) -> std::unique_ptr<FxParameter> {
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
