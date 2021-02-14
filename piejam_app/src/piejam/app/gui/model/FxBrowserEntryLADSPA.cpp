// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/FxBrowserEntryLADSPA.h>

#include <piejam/runtime/actions/insert_fx_module.h>
#include <piejam/runtime/actions/replace_fx_module.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

FxBrowserEntryLADSPA::FxBrowserEntryLADSPA(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        audio::ladspa::plugin_descriptor const& plugin_desc)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_pd(plugin_desc)
{
    static QString s_section_ladspa{tr("LADSPA")};

    setName(QString::fromStdString(m_pd.name));
    setSection(s_section_ladspa);
    if (!m_pd.name.empty())
        setAuthor(QString::fromStdString(m_pd.author));
    if (!m_pd.copyright.empty())
        setDescription(QString("Copyright / License: %1")
                               .arg(QString::fromStdString(m_pd.copyright)));
}

void
FxBrowserEntryLADSPA::onSubscribe()
{
    observe(runtime::selectors::select_fx_chain_bus,
            [this](runtime::mixer::bus_id fx_chain_bus) {
                m_fx_chain_bus = fx_chain_bus;
            });
}

void
FxBrowserEntryLADSPA::insertModule(unsigned pos)
{
    runtime::actions::load_ladspa_fx_plugin action;
    action.fx_chain_bus = m_fx_chain_bus;
    action.position = pos;
    action.plugin_id = m_pd.id;
    action.name = m_pd.name;
    dispatch(action);
}

void
FxBrowserEntryLADSPA::replaceModule(unsigned pos)
{
    dispatch(runtime::actions::replace_fx_module(pos, m_pd.id, m_pd.name));
}

} // namespace piejam::app::gui::model
