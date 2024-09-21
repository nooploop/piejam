// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxGenericModule.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/Parameter.h>
#include <piejam/gui/model/ParameterId.h>
#include <piejam/runtime/selectors.h>

#include <boost/container/flat_map.hpp>
#include <boost/hof/construct.hpp>

#include <ranges>

namespace piejam::gui::model
{

namespace
{

auto
fxParameterIds(runtime::fx::module_parameters const& params)
{
    return algorithm::transform_to_vector(
            params | std::views::values,
            boost::hof::construct<ParameterId>());
}

} // namespace

struct FxGenericModule::Impl
{
    runtime::fx::module_id fx_mod_id;

    box<runtime::fx::module_parameters> param_ids;
    FxParametersList parameters;
};

FxGenericModule::FxGenericModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
{
}

FxGenericModule::~FxGenericModule() = default;

auto
FxGenericModule::parameters() noexcept -> FxParametersList*
{
    return &m_impl->parameters;
}

auto
FxGenericModule::parameters() const noexcept -> FxParametersList const*
{
    return &m_impl->parameters;
}

void
FxGenericModule::onSubscribe()
{
    observe(runtime::selectors::make_fx_module_parameters_selector(
                    m_impl->fx_mod_id),
            [this](auto const& paramIds) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                fxParameterIds(*m_impl->param_ids),
                                fxParameterIds(*paramIds)),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *parameters(),
                                [this](ParameterId const& paramId) {
                                    auto param = model::makeParameter(
                                            dispatch(),
                                            state_change_subscriber(),
                                            paramId);
                                    connectSubscribableChild(*param);
                                    return param;
                                }});

                m_impl->param_ids = paramIds;
            });
}

} // namespace piejam::gui::model
