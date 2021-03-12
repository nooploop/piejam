// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/process_function.h>
#include <piejam/thread/fwd.h>

namespace piejam::audio
{

class device
{
public:
    virtual ~device() = default;

    virtual bool is_open() const noexcept = 0;
    virtual void close() = 0;

    virtual bool is_running() const noexcept = 0;

    virtual void
    start(thread::configuration const&,
          init_process_function const&,
          process_function) = 0;

    virtual void stop() = 0;

    virtual auto cpu_load() const noexcept -> float = 0;
    virtual auto xruns() const noexcept -> std::size_t = 0;
};

class dummy_device final : public device
{
public:
    bool is_open() const noexcept override { return false; }
    void close() override {}

    bool is_running() const noexcept override { return false; }

    void
    start(thread::configuration const&,
          init_process_function const&,
          process_function) override
    {
    }

    void stop() override {}

    auto cpu_load() const noexcept -> float override { return 0.f; }
    auto xruns() const noexcept -> std::size_t override { return 0; }
};

} // namespace piejam::audio
