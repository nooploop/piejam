// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxBrowserEntryInternal.h>

#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/replace_fx_module.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::gui::model
{

FxBrowserEntryInternal::FxBrowserEntryInternal(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::internal const fx_type)
    : FxBrowserEntry(store_dispatch, state_change_subscriber)
    , m_fx_type(fx_type)
{
    static QString s_section_internal{tr("Internal")};
    setSection(s_section_internal);

    switch (fx_type)
    {
        case runtime::fx::internal::gain:
            setName(tr("Gain"));
            setDescription(tr("Amplify or attenuate a signal."));
            break;

        case runtime::fx::internal::filter:
            setName(tr("Filter"));
            setDescription(tr("Filter a signal."));
            break;

        case runtime::fx::internal::scope:
            setName(tr("Oscilloscope"));
            setDescription(tr("Observe audio signal."));
            break;

        case runtime::fx::internal::spectrum:
            setName(tr("Spectrum"));
            setDescription(tr("Analyze frequency content of a signal."));
            break;
    }
}

void
FxBrowserEntryInternal::onSubscribe()
{
}

void
FxBrowserEntryInternal::insertModule(
        unsigned const chainIndex,
        unsigned const pos)
{
    runtime::actions::insert_internal_fx_module action;
    action.fx_chain_id = chainIdFromIndex(chainIndex);
    action.position = pos;
    action.type = m_fx_type;
    dispatch(action);
}

void
FxBrowserEntryInternal::replaceModule(
        unsigned const chainIndex,
        unsigned const pos)
{
    dispatch(runtime::actions::replace_fx_module(
            chainIdFromIndex(chainIndex),
            pos,
            m_fx_type));
}

} // namespace piejam::gui::model
