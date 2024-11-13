// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/String.h>

#include <piejam/runtime/actions/set_string.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

String::String(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::string_id string_id)
    : Subscribable{store_dispatch, state_change_subscriber}
    , m_string_id{string_id}
{
}

void
String::onSubscribe()
{
    observe(runtime::selectors::make_string_selector(m_string_id),
            [this](boxed_string s) { setValue(QString::fromStdString(s)); });
}

void
String::changeValue(QString str)
{
    runtime::actions::set_string action;
    action.id = m_string_id;
    action.str = box{str.toStdString()};
    dispatch(action);
}

} // namespace piejam::gui::model
