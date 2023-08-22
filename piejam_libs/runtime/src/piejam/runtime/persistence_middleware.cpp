// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence_middleware.h>

#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/generic_action_visitor.h>
#include <piejam/runtime/middleware_functors.h>
#include <piejam/runtime/persistence/access.h>
#include <piejam/runtime/persistence/session.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

#include <fstream>

namespace piejam::runtime
{

namespace
{

struct persistence_action_visitor : private generic_action_visitor<void>
{
    using base_t = runtime::generic_action_visitor<void>;

    using base_t::base_t;

    void operator()(actions::load_app_config const& a)
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
            dispatch(action);
        }
        catch (std::exception const& err)
        {
            auto const* const message = err.what();
            spdlog::error("could not load config file: {}", message);
        }
    }

    void operator()(actions::save_app_config const& a)
    {
        persistence::save_app_config(
                a.file,
                a.enabled_midi_devices,
                get_state());
    }

    void operator()(actions::load_session const& a)
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

            dispatch(actions::apply_session(persistence::load_session(in)));
        }
        catch (std::exception const& err)
        {
            auto const* const message = err.what();
            spdlog::error("load_session: {}", message);
        }
    }

    void operator()(actions::save_session const& a)
    {
        persistence::save_session(a.file, get_state());
    }
};

} // namespace

void
persistence_middleware::operator()(
        middleware_functors const& mw_fs,
        action const& a)
{
    if (auto action = dynamic_cast<actions::persistence_action const*>(&a))
    {
        auto v = ui::make_action_visitor<actions::persistence_action_visitor>(
                persistence_action_visitor{mw_fs});

        action->visit(v);
    }
    else
    {
        mw_fs.next(a);
    }
}

} // namespace piejam::runtime
