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

#include <piejam/container/allocator_alignment.h>
#include <piejam/range/table_view.h>

#include <boost/align/align_up.hpp>

#include <vector>

namespace piejam::container
{

template <class T, class Allocator = std::allocator<T>>
class table
{
public:
    static_assert(allocator_alignment_v<Allocator> % alignof(T) == 0, "");

    table() noexcept = default;
    table(std::size_t const num_rows, std::size_t const num_columns)
        : m_num_rows(num_rows)
        , m_num_columns(num_columns)
        , m_padding(
                  boost::alignment::align_up(
                          m_num_columns * sizeof(T),
                          allocator_alignment_v<Allocator>) /
                  sizeof(T))
        , m_data(m_num_rows * (m_num_columns + m_padding))
    {
    }

    auto num_rows() const noexcept -> std::size_t { return m_num_rows; }
    auto num_columns() const noexcept -> std::size_t { return m_num_columns; }

    auto data() const noexcept { return m_data.data(); }

    auto rows() noexcept
    {
        return range::table_view<T>(
                m_data.data(),
                m_num_rows,
                m_num_columns,
                m_num_columns + m_padding,
                1);
    }
    auto rows() const noexcept
    {
        return range::table_view<T const>(
                m_data.data(),
                m_num_rows,
                m_num_columns,
                m_num_columns + m_padding,
                1);
    }

    auto columns() noexcept
    {
        return range::table_view<T>(
                m_data.data(),
                m_num_columns,
                m_num_rows,
                1,
                m_num_columns + m_padding);
    }
    auto columns() const noexcept
    {
        return range::table_view<T const>(
                m_data.data(),
                m_num_columns,
                m_num_rows,
                1,
                m_num_columns + m_padding);
    }

private:
    std::size_t m_num_rows{};
    std::size_t m_num_columns{};
    std::size_t m_padding{};
    std::vector<T, Allocator> m_data;
};

} // namespace piejam::container
