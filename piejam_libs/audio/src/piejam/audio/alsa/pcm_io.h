// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/audio/io_process.h>
#include <piejam/audio/io_process_config.h>
#include <piejam/system/device.h>

#include <atomic>
#include <memory>

namespace piejam::audio::alsa
{

class pcm_io final : public piejam::audio::io_process
{
public:
    pcm_io() noexcept;
    pcm_io(sound_card_descriptor const& in,
           sound_card_descriptor const& out,
           io_process_config const& io_config);
    ~pcm_io() override;

    [[nodiscard]]
    auto is_open() const noexcept -> bool override;
    void close() override;

    [[nodiscard]]
    auto is_running() const noexcept -> bool override;

    void
    start(thread::configuration const&,
          init_process_function const&,
          process_function) override;

    void stop() override;

    [[nodiscard]]
    auto cpu_load() const noexcept -> float override
    {
        return m_cpu_load.load(std::memory_order_relaxed);
    }

    [[nodiscard]]
    auto xruns() const noexcept -> std::size_t override
    {
        return m_xruns.load(std::memory_order_relaxed);
    }

private:
    system::device m_input_fd;
    system::device m_output_fd;
    io_process_config m_io_config;

    std::atomic<float> m_cpu_load{};
    std::atomic_size_t m_xruns{};

    std::unique_ptr<process_thread> m_process_thread;
};

} // namespace piejam::audio::alsa
