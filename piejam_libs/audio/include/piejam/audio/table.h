// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/container/table.h>

#include <xsimd/memory/xsimd_aligned_allocator.hpp>
#include <xsimd/memory/xsimd_alignment.hpp>

namespace piejam::audio
{

using table = container::
        table<float, xsimd::aligned_allocator<float, XSIMD_DEFAULT_ALIGNMENT>>;

} // namespace piejam::audio
