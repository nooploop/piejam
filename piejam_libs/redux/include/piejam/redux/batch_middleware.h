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

#include <piejam/redux/functors.h>

#include <cassert>
#include <memory>

namespace piejam::redux
{

template <class Action>
class batch_middleware
{
public:
    batch_middleware(bool& batching, next_f<Action> next)
        : m_batching(batching)
        , m_next(std::move(next))
    {
    }

    void operator()(Action const& a) const
    {
        if (auto* const batch = as_batch_action(a))
        {
            assert(!std::empty(*batch));

            std::unique_ptr<bool, void (*)(bool*)> flag_reset(
                    &m_batching,
                    [](bool* batching) { *batching = false; });

            for (auto it = std::begin(*batch), it_end = std::end(*batch);
                 it != it_end;
                 ++it)
            {
                m_batching = std::next(it) != it_end;
                m_next(*it);
            }
        }
        else
        {
            m_next(a);
        }
    }

private:
    bool& m_batching;
    next_f<Action> m_next;
};

} // namespace piejam::redux
