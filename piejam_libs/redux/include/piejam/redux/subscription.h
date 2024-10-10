// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <nod/nod.hpp>

#include <cassert>
#include <memory>

namespace piejam::redux
{

class subscription
{
public:
    struct token
    {
    };

    subscription() = delete;
    subscription(nod::connection conn, std::shared_ptr<token> t)
        : m_conn(std::move(conn))
        , m_token(std::move(t))
    {
        assert(m_conn.connected());
        assert(m_token);
    }

    subscription(subscription const&) = delete;
    subscription(subscription&&) = default;

    ~subscription() = default;

    auto operator=(subscription&&) -> subscription& = delete;
    auto operator=(subscription const&) -> subscription& = delete;

private:
    nod::scoped_connection m_conn;
    std::shared_ptr<token> m_token;
};

} // namespace piejam::redux
