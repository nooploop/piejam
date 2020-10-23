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

#include <piejam/audio/engine/event.h>

#include <boost/assert.hpp>
#include <boost/intrusive/set.hpp>

#include <memory_resource>

namespace piejam::audio::engine
{

class abstract_event_buffer
{
public:
    virtual ~abstract_event_buffer() = default;

    virtual void clear() = 0;
};

template <class T>
class event_buffer final : public abstract_event_buffer
{
public:
    event_buffer(std::pmr::memory_resource*& event_memory)
        : m_event_memory(event_memory)
    {
    }

    bool empty() const noexcept { return m_event_container.empty(); }
    auto size() const noexcept { return m_event_container.size(); }

    auto begin() const { return m_event_container.begin(); }
    auto end() const { return m_event_container.end(); }

    void insert(std::size_t const offset, T value)
    {
        BOOST_ASSERT(m_event_memory);
        std::pmr::polymorphic_allocator<event<T>> allocator(m_event_memory);
        m_event_container.insert(*allocator.template new_object<event<T>>(
                offset,
                std::move(value)));
    }

    auto operator=(event_buffer const&) = delete;
    auto operator=(event_buffer&&) = delete;

    void clear() override { m_event_container.clear(); }

private:
    struct event_offset
    {
        using type = std::size_t;

        auto operator()(event<T> const& ev) const noexcept -> std::size_t
        {
            return ev.offset();
        }
    };

    std::pmr::memory_resource*& m_event_memory;
    boost::intrusive::
            multiset<event<T>, boost::intrusive::key_of_value<event_offset>>
                    m_event_container;
};

} // namespace piejam::audio::engine
