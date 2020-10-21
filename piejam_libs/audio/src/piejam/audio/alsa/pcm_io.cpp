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

#include <piejam/audio/alsa/pcm_io.h>

#include <piejam/audio/alsa/get_set_hw_params.h>
#include <piejam/audio/alsa/process_step.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/process_thread.h>

#include <fcntl.h>
#include <sound/asound.h>
#include <sys/ioctl.h>

namespace piejam::audio::alsa
{

static auto
open_pcm(
        std::filesystem::path const& path,
        pcm_device_config const& device_config,
        pcm_process_config const& process_config) -> io::ioctl_device
{
    if (!path.empty())
    {
        io::ioctl_device fd(path);

        set_hw_params(fd, device_config, process_config);

        unsigned const buffer_size =
                process_config.period_size * process_config.period_count;
        snd_pcm_sw_params sw_params{};
        sw_params.proto = SNDRV_PCM_VERSION;
        sw_params.tstamp_mode = SNDRV_PCM_TSTAMP_ENABLE;
        sw_params.tstamp_type = SNDRV_PCM_TSTAMP_TYPE_MONOTONIC_RAW;
        sw_params.period_step = 1;
        sw_params.sleep_min = 0;
        sw_params.avail_min = process_config.period_size;
        sw_params.xfer_align = 1;
        sw_params.start_threshold = buffer_size;
        sw_params.stop_threshold = buffer_size;
        sw_params.silence_threshold = 0;
        sw_params.boundary = buffer_size;
        while (sw_params.boundary * 2 <=
               static_cast<long unsigned>(
                       std::numeric_limits<long>::max() - buffer_size))
            sw_params.boundary *= 2;
        sw_params.silence_size = sw_params.boundary;
        fd.ioctl(SNDRV_PCM_IOCTL_SW_PARAMS, sw_params);

        fd.ioctl(SNDRV_PCM_IOCTL_PREPARE);

        return fd;
    }

    return {};
}

pcm_io::pcm_io() noexcept = default;

pcm_io::pcm_io(
        pcm_descriptor const& in,
        pcm_descriptor const& out,
        pcm_io_config const& io_config)
    : m_input_fd(
              open_pcm(in.path, io_config.in_config, io_config.process_config))
    , m_output_fd(open_pcm(
              out.path,
              io_config.out_config,
              io_config.process_config))
    , m_io_config(io_config)
{
    if (m_input_fd && m_output_fd)
        m_input_fd.ioctl(SNDRV_PCM_IOCTL_LINK, std::as_const(m_output_fd));
}

pcm_io::~pcm_io()
{
    if (is_running())
        stop();

    if (is_open())
        close();
}

bool
pcm_io::is_open() const noexcept
{
    return m_input_fd || m_output_fd;
}

void
pcm_io::close()
{
    assert(!is_running());

    auto input_fd = std::move(m_input_fd);
    auto output_fd = std::move(m_output_fd);

    if (input_fd && output_fd)
        output_fd.ioctl(SNDRV_PCM_IOCTL_UNLINK);
}

bool
pcm_io::is_running() const noexcept
{
    return m_process_thread && m_process_thread->is_running();
}

void
pcm_io::start(int const affinity, process_function process_function)
{
    assert(is_open());
    assert(!m_process_thread);

    m_xruns.store(0, std::memory_order_relaxed);

    m_process_thread = std::make_unique<process_thread>();
    m_process_thread->start(
            affinity,
            process_step(
                    m_input_fd,
                    m_output_fd,
                    m_io_config,
                    m_cpu_load,
                    m_xruns,
                    std::move(process_function)));

    assert(m_process_thread->is_running());
}

void
pcm_io::stop()
{
    assert(is_open());
    assert(m_process_thread);
    assert(m_process_thread->is_running());

    m_process_thread->stop();
    m_process_thread.reset();

    io::ioctl_device& fd = m_input_fd ? m_input_fd : m_output_fd;
    fd.ioctl(SNDRV_PCM_IOCTL_DROP);
}

} // namespace piejam::audio::alsa
