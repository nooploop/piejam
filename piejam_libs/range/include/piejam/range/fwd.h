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

template <class T, std::size_t SpanExtent>
class span_iterator;

} // namespace piejam::range
