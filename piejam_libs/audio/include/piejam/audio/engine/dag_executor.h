// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::audio::engine
{

class dag_executor
{
public:
    virtual ~dag_executor() = default;

    virtual void operator()(std::size_t buffer_size) = 0;
};

} // namespace piejam::audio::engine
