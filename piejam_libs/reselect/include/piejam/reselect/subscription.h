// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <nod/nod.hpp>

#include <cassert>
#include <memory>

namespace piejam::reselect
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

    auto operator=(subscription &&) -> subscription& = delete;
    auto operator=(subscription const&) -> subscription& = delete;

private:
    nod::scoped_connection m_conn;
    std::shared_ptr<token> m_token;
};

} // namespace piejam::reselect
