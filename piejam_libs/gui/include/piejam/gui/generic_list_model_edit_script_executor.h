// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/edit_script.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

namespace piejam::gui
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

} // namespace piejam::gui
