// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/process_function.h>
#include <piejam/thread/fwd.h>

#include <memory>

namespace piejam::audio
{

class io_process
{
public:
    virtual ~io_process() = default;

    [[nodiscard]] virtual auto is_open() const noexcept -> bool = 0;

    virtual void close() = 0;

    [[nodiscard]] virtual auto is_running() const noexcept -> bool = 0;

    virtual void
    start(thread::configuration const&,
          init_process_function const&,
          process_function) = 0;

    virtual void stop() = 0;

    [[nodiscard]] virtual auto cpu_load() const noexcept -> float = 0;
    [[nodiscard]] virtual auto xruns() const noexcept -> std::size_t = 0;
};

auto make_dummy_io_process() -> std::unique_ptr<io_process>;

} // namespace piejam::audio
