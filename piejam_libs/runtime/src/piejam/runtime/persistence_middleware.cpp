// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence_middleware.h>

#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/persistence/access.h>
#include <piejam/runtime/persistence/session.h>

#include <boost/assert.hpp>

namespace piejam::runtime
{

void
persistence_middleware::operator()(
        middleware_functors const& mw_fs,
        action const& a)
{
    if (auto action = dynamic_cast<actions::persistence_action const*>(&a))
    {
        auto v = ui::make_action_visitor<actions::persistence_action_visitor>(
                [&](auto&& a) { process_persistence_action(mw_fs, a); });

        action->visit(v);
    }
    else
    {
        mw_fs.next(a);
    }
}

template <>
void
persistence_middleware::process_persistence_action(
        middleware_functors const& mw_fs,
        actions::load_app_config const& a)
{
    persistence::load_app_config(a.file, [&](auto const& action) {
        mw_fs.dispatch(action);
    });
}

template <>
void
persistence_middleware::process_persistence_action(
        middleware_functors const& mw_fs,
        actions::save_app_config const& a)
{
    persistence::save_app_config(
            a.file,
            a.enabled_midi_devices,
            mw_fs.get_state());
}

template <>
void
persistence_middleware::process_persistence_action(
        middleware_functors const& mw_fs,
        actions::load_session const& a)
{
    persistence::load_session(a.file, [&](auto const& action) {
        mw_fs.dispatch(action);
    });
}

template <>
void
persistence_middleware::process_persistence_action(
        middleware_functors const& mw_fs,
        actions::save_session const& a)
{
    persistence::save_session(a.file, mw_fs.get_state());
}

} // namespace piejam::runtime
