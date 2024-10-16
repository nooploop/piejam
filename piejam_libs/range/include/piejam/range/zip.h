// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <ranges>

namespace piejam::range
{

template <std::ranges::range... Rs>
[[nodiscard]]
constexpr auto
zip(Rs&&... rs)
    requires(std::is_lvalue_reference_v<Rs> && ...)
{
    return boost::make_iterator_range(
            boost::make_zip_iterator(
                    boost::make_tuple(std::ranges::begin(rs)...)),
            boost::make_zip_iterator(
                    boost::make_tuple(std::ranges::end(rs)...)));
}

} // namespace piejam::range
