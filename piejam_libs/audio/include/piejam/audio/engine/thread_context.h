// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory_resource>

namespace piejam::audio::engine
{

struct thread_context
{
    std::pmr::memory_resource* event_memory{std::pmr::get_default_resource()};
};

} // namespace piejam::audio::engine
