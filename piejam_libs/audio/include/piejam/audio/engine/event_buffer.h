// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event.h>

#include <boost/assert.hpp>
#include <boost/intrusive/set.hpp>

#include <concepts>
#include <memory_resource>
#include <typeindex>

namespace piejam::audio::engine
{

class abstract_event_buffer
{
public:
    virtual ~abstract_event_buffer() = default;

    [[nodiscard]] virtual auto type() const -> std::type_index const& = 0;

    virtual void clear() = 0;
};

template <class T>
class event_buffer final : public abstract_event_buffer
{
    struct event_offset
    {
        using type = std::size_t;

        auto operator()(event<T> const& ev) const noexcept -> std::size_t
        {
            return ev.offset();
        }
    };

    using event_container_t = boost::intrusive::
            multiset<event<T>, boost::intrusive::key_of_value<event_offset>>;

public:
    using const_iterator = typename event_container_t::const_iterator;

    event_buffer(std::pmr::memory_resource*& event_memory)
        : m_event_memory(event_memory)
    {
    }

    [[nodiscard]] auto type() const -> std::type_index const& override
    {
        static std::type_index const s_type(typeid(T));
        return s_type;
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_event_container.empty();
    }

    [[nodiscard]] auto size() const noexcept
    {
        return m_event_container.size();
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator
    {
        return m_event_container.begin();
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator
    {
        return m_event_container.end();
    }

    template <std::convertible_to<T> V>
    void insert(std::size_t const offset, V&& value)
    {
        BOOST_ASSERT(m_event_memory);
        std::pmr::polymorphic_allocator<event<T>> allocator(m_event_memory);
        m_event_container.insert(*allocator.template new_object<event<T>>(
                offset,
                std::forward<V>(value)));
    }

    auto operator=(event_buffer const&) = delete;
    auto operator=(event_buffer&&) = delete;

    void clear() override
    {
        m_event_container.clear();
    }

private:
    std::pmr::memory_resource*& m_event_memory;
    event_container_t m_event_container;
};

} // namespace piejam::audio::engine
