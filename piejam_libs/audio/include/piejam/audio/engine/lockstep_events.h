// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_buffer.h>

#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <tuple>

namespace piejam::audio::engine
{

namespace detail
{

template <class F, class... T, std::size_t... I>
auto
lockstep_events(
        F&& f,
        std::tuple<T...> init,
        std::tuple<typename event_buffer<T>::const_iterator...> first,
        std::tuple<typename event_buffer<T>::const_iterator...> const last,
        std::index_sequence<I...>) -> std::tuple<T...>
{
    constexpr auto max_offset = std::numeric_limits<std::size_t>::max();

    auto const offset_from_iterator = [](auto&& f, auto&& l) {
        return f == l ? max_offset : f->offset();
    };

    while (first != last)
    {
        std::array const offsets{
                offset_from_iterator(std::get<I>(first), std::get<I>(last))...};

        std::size_t const min_offset = std::ranges::min(offsets);

        init = {
                (min_offset == offsets[I] ? (*std::get<I>(first)++).value()
                                          : std::get<I>(init))...};

        std::apply(std::bind_front(f, min_offset), init);
    }

    return init;
}

} // namespace detail

template <class F, class... T>
auto
lockstep_events(F&& f, std::tuple<T...> init, event_buffer<T> const&... ev_buf)
        -> std::tuple<T...>
{
    return detail::lockstep_events(
            std::forward<F>(f),
            std::move(init),
            std::tuple(ev_buf.begin()...),
            std::tuple(ev_buf.end()...),
            std::index_sequence_for<T...>{});
}

template <class F, class... T>
auto
lockstep_events(
        F&& f,
        std::tuple<T...> init,
        std::tuple<event_buffer<T> const&...> ev_bufs) -> std::tuple<T...>
{
    using lockstep_events_t = std::tuple<T...> (*)(
            F&&,
            std::tuple<T...>,
            event_buffer<T> const&...);
    return std::apply(
            std::bind_front(
                    static_cast<lockstep_events_t>(&lockstep_events<F, T...>),
                    std::forward<F>(f),
                    std::move(init)),
            ev_bufs);
}

} // namespace piejam::audio::engine
