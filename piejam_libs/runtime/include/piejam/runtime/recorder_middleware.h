// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/middleware_functors.h>

#include <filesystem>
#include <memory>

namespace piejam::runtime
{

class recorder_middleware final : private middleware_functors
{
public:
    recorder_middleware(
            middleware_functors,
            std::filesystem::path recordings_dir);
    ~recorder_middleware();

    void operator()(action const&);

private:
    void process_recorder_action(actions::start_recording const&);
    void process_recorder_action(actions::stop_recording const&);
    void process_recorder_action(actions::update_streams const&);

    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::runtime
