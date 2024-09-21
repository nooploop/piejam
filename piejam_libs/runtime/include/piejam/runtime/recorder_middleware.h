// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fwd.h>

#include <filesystem>
#include <memory>

namespace piejam::runtime
{

class recorder_middleware final
{
public:
    recorder_middleware(std::filesystem::path recordings_dir);
    ~recorder_middleware();

    void operator()(middleware_functors const&, action const&);

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::runtime
