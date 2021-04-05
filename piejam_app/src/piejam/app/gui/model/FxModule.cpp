// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxModule.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/app/gui/model/FxParameter.h>
#include <piejam/app/gui/model/FxParameterKeyId.h>
#include <piejam/app/gui/model/FxStream.h>
#include <piejam/app/gui/model/FxStreamKeyId.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/actions/fx_chain_actions.h>
#include <piejam/runtime/actions/move_fx_module.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/selectors.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/hof/construct.hpp>
#include <boost/hof/unpack.hpp>

namespace piejam::app::gui::model
{

namespace
{

auto
fxParameterKeyIds(runtime::fx::module_parameters const& params)
{
    return algorithm::transform_to_vector(
            params,
            boost::hof::unpack(boost::hof::construct<FxParameterKeyId>()));
}

auto
fxStreamKeyIds(runtime::fx::module_streams const& streams)
{
    return algorithm::transform_to_vector(
            streams,
            boost::hof::unpack(boost::hof::construct<FxStreamKeyId>()));
}

} // namespace

struct FxModule::Impl
{
    runtime::fx::module_id const fxModId;

    box<runtime::fx::module_parameters> paramIds;
    box<runtime::fx::module_streams> streams;
};

FxModule::FxModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(fx_mod_id))
{
}

FxModule::~FxModule() = default;

void
FxModule::onSubscribe()
{
    observe(runtime::selectors::make_fx_module_name_selector(m_impl->fxModId),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_fx_module_bypass_selector(m_impl->fxModId),
            [this](bool const x) { setBypassed(x); });

    observe(runtime::selectors::make_fx_module_can_move_left_selector(
                    m_impl->fxModId),
            [this](bool const x) { setCanMoveLeft(x); });

    observe(runtime::selectors::make_fx_module_can_move_right_selector(
                    m_impl->fxModId),
            [this](bool const x) { setCanMoveRight(x); });

    observe(runtime::selectors::make_fx_module_parameters_selector(
                    m_impl->fxModId),
            [this](box<runtime::fx::module_parameters> const& paramIds) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                fxParameterKeyIds(*m_impl->paramIds),
                                fxParameterKeyIds(*paramIds)),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *parameters(),
                                [this](FxParameterKeyId const& paramKeyId) {
                                    return std::make_unique<FxParameter>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            paramKeyId);
                                }});

                m_impl->paramIds = paramIds;
            });

    observe(runtime::selectors::make_fx_module_streams_selector(
                    m_impl->fxModId),
            [this](box<runtime::fx::module_streams> const& moduleStreams) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                fxStreamKeyIds(*m_impl->streams),
                                fxStreamKeyIds(*moduleStreams)),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *streams(),
                                [this](FxStreamKeyId const& streamKeyId) {
                                    return std::make_unique<FxStream>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            streamKeyId);
                                }});

                m_impl->streams = moduleStreams;
            });
}

void
FxModule::toggleBypass()
{
    runtime::actions::toggle_fx_module_bypass action;
    action.fx_mod_id = m_impl->fxModId;
    dispatch(action);
}

void
FxModule::deleteModule()
{
    runtime::actions::delete_fx_module action;
    action.fx_mod_id = m_impl->fxModId;
    dispatch(action);
}

void
FxModule::moveLeft()
{
    BOOST_ASSERT(canMoveLeft());

    runtime::actions::move_fx_module_left action;
    action.fx_mod_id = m_impl->fxModId;
    dispatch(action);
}

void
FxModule::moveRight()
{
    BOOST_ASSERT(canMoveRight());

    runtime::actions::move_fx_module_right action;
    action.fx_mod_id = m_impl->fxModId;
    dispatch(action);
}

} // namespace piejam::app::gui::model
