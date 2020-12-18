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
#include <piejam/container/box.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::app::gui::model
{

class FxModule final : public Subscribable<piejam::gui::model::FxModule>
{
public:
    FxModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);

    void deleteModule() override;

private:
    void subscribe_step() override;

    runtime::fx::module_id const m_fx_mod_id;

    container::box<runtime::fx::module_parameters> m_param_ids;
};

} // namespace piejam::app::gui::model
