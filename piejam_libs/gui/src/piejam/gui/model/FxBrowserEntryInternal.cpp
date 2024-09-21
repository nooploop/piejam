// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxBrowserEntryInternal.h>

#include <piejam/gui/model/FxBrowserEntryInternalData.h>
#include <piejam/npos.h>
#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

FxBrowserEntryInternal::FxBrowserEntryInternal(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::internal_id const fx_type)
    : FxBrowserEntry(store_dispatch, state_change_subscriber)
    , m_fx_type(fx_type)
{
    static QString s_section_internal{tr("Internal")};
    setSection(s_section_internal);

    auto const& data = FxBrowserEntryInternalDataMap::lookup(fx_type);
    setName(data.name);
    setDescription(data.description);
}

void
FxBrowserEntryInternal::onSubscribe()
{
}

void
FxBrowserEntryInternal::appendModule()
{
    runtime::actions::insert_internal_fx_module action;
    action.fx_chain_id =
            observe_once(runtime::selectors::select_fx_browser_fx_chain);
    action.position = npos;
    action.type = m_fx_type;
    action.show_fx_module = true;
    dispatch(action);
}

} // namespace piejam::gui::model
