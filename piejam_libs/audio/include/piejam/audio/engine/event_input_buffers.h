// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_buffer.h>
#include <piejam/audio/engine/event_port.h>

#include <boost/polymorphic_cast.hpp>

#include <vector>

namespace piejam::audio::engine
{

class event_input_buffers final
{
public:
    using size_type = std::size_t;

    event_input_buffers() = default;

    auto size() const noexcept -> std::size_t { return m_event_buffers.size(); }

    auto begin() const { return m_event_buffers.begin(); }
    auto end() const { return m_event_buffers.end(); }

    void add(event_port const& port)
    {
        m_event_buffers.push_back(std::addressof(port.empty_event_buffer()));

#ifndef NDEBUG
        m_connected.push_back(false);
#endif
    }

    auto
    set(std::size_t const buffer_index, abstract_event_buffer const& ev_buf)
    {
        BOOST_ASSERT(buffer_index < m_event_buffers.size());
        BOOST_ASSERT_MSG(
                !m_connected[buffer_index],
                "Input already connected, event_merger needed?");
        BOOST_ASSERT(m_event_buffers[buffer_index]->type() == ev_buf.type());
        m_event_buffers[buffer_index] = std::addressof(ev_buf);

#ifndef NDEBUG
        m_connected[buffer_index] = true;
#endif
    }

    template <class T>
    auto get(std::size_t const buffer_index) const noexcept
            -> event_buffer<T> const&
    {
        BOOST_ASSERT(buffer_index < m_event_buffers.size());
        abstract_event_buffer const* const ev_buf =
                m_event_buffers[buffer_index];

        return *boost::polymorphic_downcast<event_buffer<T> const*>(ev_buf);
    }

private:
    std::vector<abstract_event_buffer const*> m_event_buffers;

#ifndef NDEBUG
    std::vector<bool> m_connected;
#endif
};

} // namespace piejam::audio::engine
