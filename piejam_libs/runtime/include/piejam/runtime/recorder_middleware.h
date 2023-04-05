// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
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
    void process_recorder_action(
            middleware_functors const&,
            actions::start_recording const&);
    void process_recorder_action(
            middleware_functors const&,
            actions::stop_recording const&);
    void process_recorder_action(
            middleware_functors const&,
            actions::update_streams const&);

    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::runtime
