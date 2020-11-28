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

#include <piejam/algorithm/edit_script.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

namespace piejam::app::gui
{

template <class ListItem, class ListItemFactory>
struct generic_list_model_edit_script_executor
{
    piejam::gui::model::GenericListModel<ListItem>& list;
    ListItemFactory make_list_item;

    void operator()(algorithm::edit_script_deletion const& del) const
    {
        list.remove(del.pos);
    }

    template <class EditValue>
    void
    operator()(algorithm::edit_script_insertion<EditValue> const& ins) const
    {
        list.add(ins.pos, make_list_item(ins.value));
    }
};

template <class ListItem, class ListItemFactory>
generic_list_model_edit_script_executor(
        piejam::gui::model::GenericListModel<ListItem>&,
        ListItemFactory&&)
        -> generic_list_model_edit_script_executor<ListItem, ListItemFactory>;

} // namespace piejam::app::gui
