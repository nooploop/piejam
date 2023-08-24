// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxParameter.h>

#include <piejam/gui/model/FxBoolParameter.h>
#include <piejam/gui/model/FxFloatParameter.h>
#include <piejam/gui/model/FxIntParameter.h>
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

#include <boost/hof/match.hpp>

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
    setName(QString::fromStdString(*observe_once(
            runtime::selectors::make_fx_parameter_name_selector(param.id))));
}

FxParameter::~FxParameter() = default;

auto
FxParameter::type() const noexcept -> Type
{
    return std::visit(
            boost::hof::match(
                    [&](runtime::bool_parameter_id const&) {
                        return Type::Bool;
                    },
                    [&](runtime::float_parameter_id const&) {
                        return Type::Float;
                    },
                    [&](runtime::int_parameter_id const&) {
                        return Type::Int;
                    }),
            m_impl->param.id);
}

void
FxParameter::onSubscribe()
{
    observe(runtime::selectors::make_fx_parameter_value_string_selector(
                    m_impl->param.id),
            [this](std::string const& text) {
                setValueString(QString::fromStdString(text));
            });
}

auto
FxParameter::midi() const -> MidiAssignable*
{
    return m_impl->midi.get();
}

auto
FxParameter::paramKeyId() const -> FxParameterKeyId
{
    return m_impl->param;
}

auto
makeFxParameter(
        runtime::store_dispatch dispatch,
        runtime::subscriber& state_change_subscriber,
        piejam::gui::model::FxParameterKeyId const& paramKeyId)
        -> std::unique_ptr<FxParameter>
{
    return std::visit(
            boost::hof::match(
                    [&](runtime::bool_parameter_id const&)
                            -> std::unique_ptr<FxParameter> {
                        return std::make_unique<FxBoolParameter>(
                                dispatch,
                                state_change_subscriber,
                                paramKeyId);
                    },
                    [&](runtime::float_parameter_id const&)
                            -> std::unique_ptr<FxParameter> {
                        return std::make_unique<FxFloatParameter>(
                                dispatch,
                                state_change_subscriber,
                                paramKeyId);
                    },
                    [&](runtime::int_parameter_id const&)
                            -> std::unique_ptr<FxParameter> {
                        return std::make_unique<FxIntParameter>(
                                dispatch,
                                state_change_subscriber,
                                paramKeyId);
                    }),
            paramKeyId.id);
}

} // namespace piejam::gui::model
