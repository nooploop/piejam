// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/device.h>
#include <piejam/audio/fwd.h>
#include <piejam/audio/pcm_io_config.h>
#include <piejam/system/ioctl_device.h>

#include <atomic>
#include <memory>

namespace piejam::audio::alsa
{

class pcm_io : public piejam::audio::device
{
public:
    pcm_io() noexcept;
    pcm_io(pcm_descriptor const& in,
           pcm_descriptor const& out,
           pcm_io_config const& io_config);
    ~pcm_io() override;

    bool is_open() const noexcept override;
    void close() override;

    bool is_running() const noexcept override;
    void start(thread::configuration const&, process_function) override;
    void stop() override;

    auto cpu_load() const noexcept -> float override
    {
        return m_cpu_load.load(std::memory_order_relaxed);
    }
    auto xruns() const noexcept -> std::size_t override
    {
        return m_xruns.load(std::memory_order_relaxed);
    }

private:
    system::ioctl_device m_input_fd;
    system::ioctl_device m_output_fd;
    pcm_io_config m_io_config;

    std::atomic<float> m_cpu_load{};
    std::atomic_size_t m_xruns{};

    std::unique_ptr<process_thread> m_process_thread;
};

} // namespace piejam::audio::alsa
