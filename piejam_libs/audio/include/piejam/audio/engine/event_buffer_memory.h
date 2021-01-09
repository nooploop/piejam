// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <memory_resource>
#include <vector>

namespace piejam::audio::engine
{

class event_buffer_memory final
{
public:
    event_buffer_memory(std::size_t const initial_size)
        : m_memory(initial_size)
    {
    }

    auto memory_resource() noexcept -> std::pmr::memory_resource&
    {
        return *m_memory_resource;
    }

    void release() { m_memory_resource->release(); }

private:
    std::vector<std::byte> m_memory;
    std::unique_ptr<std::pmr::monotonic_buffer_resource> m_memory_resource{
            std::make_unique<std::pmr::monotonic_buffer_resource>(
                    m_memory.data(),
                    m_memory.size())};
};

} // namespace piejam::audio::engine
