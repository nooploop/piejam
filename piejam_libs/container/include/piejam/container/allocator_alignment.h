// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
