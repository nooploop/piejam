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

#include <xsimd/memory/xsimd_aligned_allocator.hpp>

#include <boost/align/aligned_allocator_forward.hpp>

#include <cstddef>
#include <memory>

namespace piejam::container
{

template <class Allocator>
struct allocator_alignment;

template <class T>
struct allocator_alignment<std::allocator<T>>
{
    static constexpr std::size_t value = alignof(std::max_align_t);
};

template <class T, std::size_t Alignment>
struct allocator_alignment<boost::alignment::aligned_allocator<T, Alignment>>
{
    static constexpr std::size_t value = Alignment;
};

template <class T, std::size_t Alignment>
struct allocator_alignment<xsimd::aligned_allocator<T, Alignment>>
{
    static constexpr std::size_t value = Alignment;
};

template <class Allocator>
inline constexpr std::size_t allocator_alignment_v =
        allocator_alignment<Allocator>::value;

} // namespace piejam::container
