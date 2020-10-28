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

#include <algorithm>
#include <limits>
#include <tuple>

namespace piejam::audio::engine
{

namespace detail
{

template <class F, class InitTuple, class IteratorTuple, std::size_t... I>
auto
lockstep_events(
        F&& f,
        InitTuple init,
        IteratorTuple first,
        IteratorTuple const last,
        std::index_sequence<I...>)
{
    constexpr auto max_offset = std::numeric_limits<std::size_t>::max();
    auto const offset_from_iterator = [](auto&& f, auto&& l) {
        return f == l ? max_offset : f->offset();
    };
    auto const get_value_and_advance = [](auto& it) { return (*it++).value(); };
    while (first != last)
    {
        auto offsets = std::tuple(
                offset_from_iterator(std::get<I>(first), std::get<I>(last))...);
        std::size_t const min_offset = std::min({std::get<I>(offsets)...});
        init = {
                (min_offset == std::get<I>(offsets)
                         ? get_value_and_advance(std::get<I>(first))
                         : std::get<I>(init))...};
        std::apply(
                [&f, min_offset](auto&&... args) {
                    std::invoke(
                            f,
                            min_offset,
                            std::forward<decltype(args)>(args)...);
                },
                init);
    }

    return init;
}

} // namespace detail

template <class F, class... T>
auto
lockstep_events(F&& f, std::tuple<T...> init, event_buffer<T> const&... ev_buf)
{
    return detail::lockstep_events(
            std::forward<F>(f),
            std::move(init),
            std::tuple(ev_buf.begin()...),
            std::tuple(ev_buf.end()...),
            std::index_sequence_for<T...>{});
}

} // namespace piejam::audio::engine
