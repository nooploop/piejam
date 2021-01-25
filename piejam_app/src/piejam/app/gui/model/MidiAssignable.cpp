// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/MidiAssignable.h>

#include <piejam/runtime/actions/control_midi_assignment.h>
#include <piejam/runtime/selectors.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

namespace piejam::app::gui::model
{

static auto
toQString(runtime::midi_assignment const& ass)
{
    switch (ass.control_type)
    {
        case runtime::midi_assignment::type::cc:
            return QString("CC %1").arg(ass.control_id);

        case runtime::midi_assignment::type::pc:
            return QString("PC %1").arg(ass.control_id);
    }

    __builtin_unreachable();
}

MidiAssignable::MidiAssignable(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::midi_assignment_id const& assignment_id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_assignment_id(assignment_id)
{
    BOOST_ASSERT(runtime::is_valid_midi_assignment_id(m_assignment_id));
}

void
MidiAssignable::onSubscribe()
{
    observe(runtime::selectors::make_midi_assignment_selector(m_assignment_id),
            [this](std::optional<runtime::midi_assignment> const& ass) {
                setAssignment(ass ? toQString(*ass) : QString());
            });
}

void
MidiAssignable::startLearn()
{
    runtime::actions::start_midi_learning action;
    action.assignment_id = m_assignment_id;
    dispatch(action);
}

void
MidiAssignable::stopLearn()
{
    dispatch(runtime::actions::stop_midi_learning{});
}

} // namespace piejam::app::gui::model
