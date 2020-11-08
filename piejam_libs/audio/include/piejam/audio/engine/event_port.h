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

#include <piejam/audio/engine/event_buffer.h>

#include <string>
#include <string_view>
#include <typeindex>
#include <utility>

namespace piejam::audio::engine
{

class event_port
{
public:
    template <class T>
    event_port(std::in_place_type_t<T>, std::string_view const& name = {})
        : m_type(typeid(T))
        , m_name(name)
        , m_make_event_buffer(&make_event_buffer<T>)
        , m_empty_event_buffer(&empty_event_buffer<T>)
    {
    }

    auto type() const noexcept -> std::type_index const& { return m_type; }
    auto name() const noexcept -> std::string const& { return m_name; }

    auto make_event_buffer(std::pmr::memory_resource*& event_memory) const
            -> std::unique_ptr<abstract_event_buffer>
    {
        return m_make_event_buffer(event_memory);
    }

    auto empty_event_buffer() const -> abstract_event_buffer const&
    {
        return m_empty_event_buffer();
    }

private:
    template <class T>
    static auto make_event_buffer(std::pmr::memory_resource*& event_memory)
            -> std::unique_ptr<abstract_event_buffer>
    {
        return std::make_unique<event_buffer<T>>(event_memory);
    }

    template <class T>
    static auto empty_event_buffer() -> abstract_event_buffer const&
    {
        static std::pmr::memory_resource* s_event_memory{};
        static event_buffer<T> const s_empty_event_buffer(s_event_memory);
        return s_empty_event_buffer;
    }

    using make_event_buffer_fn_t = std::unique_ptr<abstract_event_buffer> (*)(
            std::pmr::memory_resource*&);

    using empty_event_buffer_fn_t = abstract_event_buffer const& (*)();

    std::type_index m_type;
    std::string m_name;

    make_event_buffer_fn_t m_make_event_buffer;
    empty_event_buffer_fn_t m_empty_event_buffer;
};

} // namespace piejam::audio::engine
