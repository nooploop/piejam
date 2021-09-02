// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>

#include <iterator>

namespace piejam
{

template <class Container, class T>
auto
insert_at(Container&& c, std::size_t const index, T&& value)
{
    BOOST_ASSERT(index <= c.size());

    return std::forward<Container>(c).insert(
            std::next(std::begin(std::forward<Container>(c)), index),
            std::forward<T>(value));
}

template <class Container>
auto
erase_at(Container&& c, std::size_t const index)
{
    BOOST_ASSERT(index < c.size());

    return std::forward<Container>(c).erase(
            std::next(std::begin(std::forward<Container>(c)), index));
}

} // namespace piejam
