// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::range
{

template <class Iterator>
class stride_iterator;

template <class T>
class strided_span;

template <class T>
class table_view;

inline constexpr std::size_t dynamic_channels = 0;

template <class T, std::size_t NumChannels>
struct frame_iterator;
template <class T, std::size_t NumChannels>
struct interleaved_view;
template <class T>
using dynamic_interleaved_view = interleaved_view<T, dynamic_channels>;
template <class T, std::size_t NumChannels>
class interleaved_vector;

} // namespace piejam::range
