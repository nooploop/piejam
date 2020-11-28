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

#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

FxParameter::FxParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id fx_mod_id,
        runtime::fx::parameter_key fx_param_key)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_fx_mod_id(fx_mod_id)
    , m_fx_param_key(fx_param_key)
{
}

void
FxParameter::subscribe_step()
{
    observe(runtime::selectors::make_fx_parameter_name_selector(
                    m_fx_mod_id,
                    m_fx_param_key),
            [this](container::boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });
}

} // namespace piejam::app::gui::model
