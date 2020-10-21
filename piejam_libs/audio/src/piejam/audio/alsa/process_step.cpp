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

#include <piejam/audio/alsa/process_step.h>

#include <piejam/audio/alsa/pcm_reader.h>
#include <piejam/audio/alsa/pcm_writer.h>
#include <piejam/audio/cpu_load_meter.h>
#include <piejam/audio/pcm_convert.h>
#include <piejam/audio/pcm_format.h>
#include <piejam/audio/pcm_io_config.h>
#include <piejam/audio/pcm_sample_type.h>
#include <piejam/container/table.h>
#include <piejam/io/ioctl_device.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <cassert>

namespace piejam::audio::alsa
{

namespace
{

template <class T>
void
transferi(
        io::ioctl_device const& fd,
        unsigned long const request,
        T* const buffer,
        std::size_t const frames,
        std::size_t const channels_per_frame)
{
    assert(buffer);

    auto step =
            [&fd,
             request](void* const buffer, std::size_t frames) -> std::size_t {
        snd_xferi arg;
        arg.buf = buffer;
        arg.frames = frames;
        arg.result = 0;

        fd.ioctl(request, arg);

        return static_cast<std::size_t>(arg.result);
    };

    std::size_t frames_transferred = 0;
    while (frames_transferred < frames)
    {
        T* const place = buffer + frames_transferred * channels_per_frame;
        std::size_t const frames_left = frames - frames_transferred;
        frames_transferred += step(place, frames_left);
    }
}

template <class T>
void
readi(io::ioctl_device const& fd,
      T* const buffer,
      std::size_t const frames,
      std::size_t const channels_per_frame)
{
    transferi(
            fd,
            SNDRV_PCM_IOCTL_READI_FRAMES,
            buffer,
            frames,
            channels_per_frame);
}

template <class T>
void
writei(io::ioctl_device const& fd,
       T* const buffer,
       std::size_t const frames,
       std::size_t const channels_per_frame)
{
    transferi(
            fd,
            SNDRV_PCM_IOCTL_WRITEI_FRAMES,
            buffer,
            frames,
            channels_per_frame);
}

struct dummy_reader : pcm_reader
{
    auto buffer() const noexcept -> range::table_view<float const> override
    {
        return {};
    }

    void read() override {}
};

template <pcm_format F>
struct interleaved_reader : pcm_reader
{
    interleaved_reader(
            io::ioctl_device& fd,
            std::size_t const num_channels,
            std::size_t const period_size)
        : m_fd(fd)
        , m_num_channels(num_channels)
        , m_period_size(period_size)
        , m_read_buffer(num_channels * period_size)
        , m_buffer(num_channels, period_size)
    {
        assert(m_fd);
    }

    auto buffer() const noexcept -> range::table_view<float const> override
    {
        return m_buffer.rows();
    }

    void read() override
    {
        readi(m_fd, m_read_buffer.data(), m_period_size, m_num_channels);
        range::table_view<pcm_sample_t<F>> non_interleaved(
                m_read_buffer.data(),
                m_num_channels,
                m_period_size,
                1,
                m_num_channels);
        transform(non_interleaved, m_buffer.rows(), &pcm_convert::from<F>);
    }

private:
    io::ioctl_device& m_fd;
    std::size_t m_num_channels;
    std::size_t m_period_size;
    std::vector<pcm_sample_t<F>> m_read_buffer;
    container::table<float> m_buffer;
};

auto
make_reader(
        io::ioctl_device& fd,
        pcm_device_config const& config,
        std::size_t const period_size) -> std::unique_ptr<pcm_reader>
{
    if (!fd)
        return std::make_unique<dummy_reader>();

    if (config.interleaved)
    {

#define M_PIEJAM_INTERLEAVED_READER_CASE(Format)                               \
    case Format:                                                               \
        return std::make_unique<interleaved_reader<Format>>(                   \
                fd,                                                            \
                config.num_channels,                                           \
                period_size)

        switch (config.format)
        {
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s8);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u8);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s16_le);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s16_be);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u16_le);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u16_be);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s32_le);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s32_be);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u32_le);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u32_be);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s24_3le);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::s24_3be);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u24_3le);
            M_PIEJAM_INTERLEAVED_READER_CASE(pcm_format::u24_3be);

            default:
                assert(false);
                return std::make_unique<dummy_reader>();
        }

#undef M_PIEJAM_INTERLEAVED_READER_CASE
    }
    else
    {
        return std::make_unique<dummy_reader>();
    }
}

struct dummy_writer : pcm_writer
{
    auto buffer() noexcept -> range::table_view<float> override { return {}; }

    void write() override {}
};

template <pcm_format F>
struct interleaved_writer : pcm_writer
{
    interleaved_writer(
            io::ioctl_device& fd,
            std::size_t const num_channels,
            std::size_t const period_size)
        : m_fd(fd)
        , m_num_channels(num_channels)
        , m_period_size(period_size)
        , m_write_buffer(num_channels * period_size)
        , m_buffer(num_channels, period_size)
    {
        assert(m_fd);
    }

    auto buffer() noexcept -> range::table_view<float> override
    {
        return m_buffer.rows();
    }

    void write() override
    {
        range::table_view<pcm_sample_t<F>> non_interleaved(
                m_write_buffer.data(),
                m_num_channels,
                m_period_size,
                1,
                m_num_channels);
        transform(m_buffer.rows(), non_interleaved, &pcm_convert::to<F>);
        writei(m_fd, m_write_buffer.data(), m_period_size, m_num_channels);
    }

private:
    io::ioctl_device& m_fd;
    std::size_t m_num_channels;
    std::size_t m_period_size;
    std::vector<pcm_sample_t<F>> m_write_buffer;
    container::table<float> m_buffer;
};

auto
make_writer(
        io::ioctl_device& fd,
        pcm_device_config const& config,
        std::size_t const period_size) -> std::unique_ptr<pcm_writer>
{
    if (!fd)
        return std::make_unique<dummy_writer>();

    if (config.interleaved)
    {

#define M_PIEJAM_INTERLEAVED_WRITER_CASE(Format)                               \
    case Format:                                                               \
        return std::make_unique<interleaved_writer<Format>>(                   \
                fd,                                                            \
                config.num_channels,                                           \
                period_size)

        switch (config.format)
        {
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s8);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u8);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s16_le);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s16_be);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u16_le);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u16_be);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s32_le);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s32_be);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u32_le);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u32_be);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s24_3le);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::s24_3be);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u24_3le);
            M_PIEJAM_INTERLEAVED_WRITER_CASE(pcm_format::u24_3be);

            default:
                assert(false);
                return std::make_unique<dummy_writer>();
        }

#undef M_PIEJAM_INTERLEAVED_WRITER_CASE
    }
    else
    {
        return std::make_unique<dummy_writer>();
    }
}

} // namespace

process_step::process_step(
        io::ioctl_device& input_fd,
        io::ioctl_device& output_fd,
        pcm_io_config const& io_config,
        std::atomic<float>& cpu_load,
        std::atomic_size_t& xruns,
        process_function process_function)
    : m_input_fd(input_fd)
    , m_output_fd(output_fd)
    , m_io_config(io_config)
    , m_cpu_load(cpu_load)
    , m_xruns(xruns)
    , m_process_function(std::move(process_function))
    , m_reader(make_reader(
              m_input_fd,
              m_io_config.in_config,
              m_io_config.process_config.period_size))
    , m_writer(make_writer(
              m_output_fd,
              m_io_config.out_config,
              m_io_config.process_config.period_size))
{
    m_xruns.store(0, std::memory_order_relaxed);
}

process_step::process_step(process_step&&) = default;

process_step::~process_step() = default;

void
process_step::operator()()
{
    if (m_starting)
    {
        if (m_output_fd)
        {
            for (auto const& row : m_writer->buffer())
                std::ranges::fill(row, 0.f);

            for (unsigned i = 0; i < m_io_config.process_config.period_count;
                 ++i)
                m_writer->write();
        }
        else if (m_input_fd)
        {
            m_input_fd.ioctl(SNDRV_PCM_IOCTL_START);
        }

        m_starting = false;
    }

    try
    {
        m_reader->read();

        cpu_load_meter cpu_load_meter(
                std::max(
                        m_reader->buffer().minor_size(),
                        m_writer->buffer().minor_size()),
                m_io_config.process_config.samplerate);

        m_process_function(m_reader->buffer(), m_writer->buffer());

        m_cpu_load.store(cpu_load_meter.stop(), std::memory_order_relaxed);

        m_writer->write();
    }
    catch (std::system_error const& err)
    {
        if (err.code() == std::make_error_code(std::errc::broken_pipe))
        {
            io::ioctl_device& fd = m_input_fd ? m_input_fd : m_output_fd;
            fd.ioctl(SNDRV_PCM_IOCTL_PREPARE);
            m_starting = true;
            ++m_xruns;
        }
        else
            throw;
    }
}

} // namespace piejam::audio::alsa
