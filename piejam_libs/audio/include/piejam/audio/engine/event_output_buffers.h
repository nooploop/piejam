// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_buffer.h>
#include <piejam/audio/engine/event_port.h>

#include <boost/assert.hpp>
#include <boost/polymorphic_cast.hpp>

#include <memory>
#include <memory_resource>
#include <vector>

namespace piejam::audio::engine
{

class event_output_buffers final
{
public:
    using size_type = std::size_t;

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_event_buffers.empty();
    }

    [[nodiscard]] auto size() const noexcept -> size_type
    {
        return m_event_buffers.size();
    }

    [[nodiscard]] auto begin() const
    {
        return m_event_buffers.begin();
    }

    [[nodiscard]] auto end() const
    {
        return m_event_buffers.end();
    }

    void set_event_memory(std::pmr::memory_resource* const event_memory)
    {
        m_event_memory = event_memory;
    }

    void add(event_port const& port)
    {
        m_event_buffers.emplace_back(port.make_event_buffer(m_event_memory));
    }

    template <class T>
    auto get(std::size_t const buffer_index) const noexcept -> event_buffer<T>&
    {
        BOOST_ASSERT(buffer_index < m_event_buffers.size());
        return *boost::polymorphic_downcast<event_buffer<T>*>(
                m_event_buffers[buffer_index].get());
    }

    auto get(std::size_t const buffer_index) const noexcept
            -> abstract_event_buffer const&
    {
        BOOST_ASSERT(buffer_index < m_event_buffers.size());
        return *m_event_buffers[buffer_index];
    }

    void clear_buffers()
    {
        for (auto& ev_buf : m_event_buffers)
        {
            ev_buf->clear();
        }
    }

private:
    std::vector<std::unique_ptr<abstract_event_buffer>> m_event_buffers;
    std::pmr::memory_resource* m_event_memory{std::pmr::get_default_resource()};
};

} // namespace piejam::audio::engine
