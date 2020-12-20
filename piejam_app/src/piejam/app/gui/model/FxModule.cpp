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

#include <piejam/app/gui/model/FxModule.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/generic_list_model_edit_script_executor.h>
#include <piejam/app/gui/model/FxParameter.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/selectors.h>

#include <boost/container/flat_map.hpp>

namespace piejam::app::gui::model
{

namespace
{

struct parameter_key_id
{
    runtime::fx::parameter_key key;
    runtime::fx::parameter_id id;

    constexpr bool operator==(parameter_key_id const& other) const noexcept
    {
        return key == other.key;
    }
};

auto
parameter_key_ids(runtime::fx::module_parameters const& params)
{
    std::vector<parameter_key_id> result;
    result.reserve(params.size());
    std::ranges::transform(
            params,
            std::back_inserter(result),
            [](auto&& mod_param) {
                return parameter_key_id{mod_param.first, mod_param.second};
            });
    return result;
}

} // namespace

FxModule::FxModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id fx_mod_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_fx_mod_id(fx_mod_id)
{
}

void
FxModule::onSubscribe()
{
    observe(runtime::selectors::make_fx_module_name_selector(m_fx_mod_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_fx_module_parameters_selector(m_fx_mod_id),
            [this](box<runtime::fx::module_parameters> const&
                           param_ids) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                parameter_key_ids(*m_param_ids),
                                parameter_key_ids(*param_ids)),
                        generic_list_model_edit_script_executor{
                                *parameters(),
                                [this](parameter_key_id const& param_key_id) {
                                    return std::make_unique<FxParameter>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            param_key_id.id);
                                }});

                m_param_ids = param_ids;
            });
}

void
FxModule::deleteModule()
{
    runtime::actions::delete_fx_module action;
    action.fx_mod_id = m_fx_mod_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
