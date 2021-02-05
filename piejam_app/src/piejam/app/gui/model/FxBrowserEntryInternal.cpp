// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxBrowserEntryInternal.h>

#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/replace_fx_module.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

FxBrowserEntryInternal::FxBrowserEntryInternal(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::internal const fx_type)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_fx_type(fx_type)
{
    static QString s_section_internal{tr("Internal")};

    switch (fx_type)
    {
        case runtime::fx::internal::gain:
            setName(tr("Gain"));
            setSection(s_section_internal);
            setDescription(tr("Amplify or attenuate a signal."));
            break;
    }
}

void
FxBrowserEntryInternal::onSubscribe()
{
    observe(runtime::selectors::select_fx_chain_bus,
            [this](runtime::mixer::bus_id fx_chain_bus) {
                m_fx_chain_bus = fx_chain_bus;
            });
}

void
FxBrowserEntryInternal::insertModule(unsigned pos)
{
    runtime::actions::insert_internal_fx_module action;
    action.fx_chain_bus = m_fx_chain_bus;
    action.position = pos;
    action.type = m_fx_type;
    dispatch(action);
}

void
FxBrowserEntryInternal::replaceModule(unsigned pos)
{
    dispatch(runtime::actions::replace_fx_module(pos, m_fx_type));
}

} // namespace piejam::app::gui::model
