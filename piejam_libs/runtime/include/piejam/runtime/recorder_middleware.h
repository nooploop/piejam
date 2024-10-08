// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <piejam/pimpl.h>

#include <filesystem>

namespace piejam::runtime
{

class recorder_middleware final
{
public:
    explicit recorder_middleware(std::filesystem::path recordings_dir);

    void operator()(middleware_functors const&, action const&);

private:
    struct impl;
    pimpl<impl> m_impl;
};

} // namespace piejam::runtime
