// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence_middleware.h>

#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/persistence/access.h>
#include <piejam/runtime/persistence/session.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

#include <fstream>

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
                [this, &mw_fs](auto const& a) {
                    process_persistence_action(mw_fs, a);
                });

        action->visit(v);
    }
    else
    {
        mw_fs.next(a);
    }
}

template <class Action>
void
persistence_middleware::process_persistence_action(
        middleware_functors const&,
        Action const&)
{
    BOOST_ASSERT_MSG(false, "unhandled action");
}

template <>
void
persistence_middleware::process_persistence_action(
        middleware_functors const& mw_fs,
        actions::load_app_config const& a)
{
    if (!std::filesystem::exists(a.file))
    {
        return;
    }

    try
    {
        std::ifstream in(a.file);
        if (!in.is_open())
        {
            throw std::runtime_error("could not open config file");
        }

        actions::apply_app_config action;
        action.conf = persistence::load_app_config(in);
        mw_fs.dispatch(action);
    }
    catch (std::exception const& err)
    {
        auto const* const message = err.what();
        spdlog::error("could not load config file: {}", message);
    }
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
    if (!std::filesystem::exists(a.file))
    {
        return;
    }

    try
    {
        std::ifstream in(a.file);
        if (!in.is_open())
        {
            throw std::runtime_error("could not open session file");
        }

        actions::apply_session action;
        action.session = persistence::load_session(in);
        mw_fs.dispatch(action);
    }
    catch (std::exception const& err)
    {
        auto const* const message = err.what();
        spdlog::error("load_session: {}", message);
    }
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
