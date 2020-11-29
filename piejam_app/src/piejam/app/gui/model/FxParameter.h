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

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/parameters.h>

namespace piejam::app::gui::model
{

class FxParameter final : public Subscribable<piejam::gui::model::FxParameter>
{
public:
    FxParameter(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id fx_mod_id,
            runtime::fx::parameter_key fx_param_key);

    void changeValue(double) override;

private:
    void subscribe_step() override;

    runtime::fx::module_id m_fx_mod_id;
    runtime::fx::parameter_key m_fx_param_key;
    runtime::float_parameter_id m_param_id;
};

} // namespace piejam::app::gui::model
