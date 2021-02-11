// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "process_step.h"

#include "pcm_reader.h"
#include "pcm_writer.h"

#include <piejam/audio/cpu_load_meter.h>
#include <piejam/audio/pcm_convert.h>
#include <piejam/audio/pcm_format.h>
#include <piejam/audio/pcm_io_config.h>
#include <piejam/audio/pcm_sample_type.h>
#include <piejam/audio/table.h>
#include <piejam/system/device.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <cassert>

namespace piejam::audio::alsa
{

namespace
{

template <class T>
auto
transferi(
        system::device& fd,
        unsigned long const request,
        T* const buffer,
        std::size_t const frames,
        std::size_t const channels_per_frame) -> std::error_code
{
    BOOST_ASSERT(buffer);

    std::size_t frames_transferred = 0;
    while (frames_transferred < frames)
    {
        snd_xferi arg;
        arg.buf = buffer + frames_transferred * channels_per_frame;
        arg.frames = frames - frames_transferred;
        arg.result = 0;

        if (auto err = fd.ioctl(request, arg))
            return err;

        frames_transferred += static_cast<std::size_t>(arg.result);
    }

    return {};
}

template <class T>
auto
readi(system::device& fd,
      T* const buffer,
      std::size_t const frames,
      std::size_t const channels_per_frame) -> std::error_code
{
    return transferi(
            fd,
            SNDRV_PCM_IOCTL_READI_FRAMES,
            buffer,
            frames,
            channels_per_frame);
}

template <class T>
auto
writei(system::device& fd,
       T* const buffer,
       std::size_t const frames,
       std::size_t const channels_per_frame) -> std::error_code
{
    return transferi(
            fd,
            SNDRV_PCM_IOCTL_WRITEI_FRAMES,
            buffer,
            frames,
            channels_per_frame);
}

struct dummy_reader final : pcm_reader
{
    auto buffer() const noexcept -> range::table_view<float const> override
    {
        return {};
    }

    auto read() noexcept -> std::error_code override { return {}; }

    void clear_buffer() noexcept override {}
};

template <pcm_format F>
struct interleaved_reader final : pcm_reader
{
    interleaved_reader(
            system::device& fd,
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

    auto read() noexcept -> std::error_code override
    {
        if (auto err =
                    readi(m_fd,
                          m_read_buffer.data(),
                          m_period_size,
                          m_num_channels))
            return err;

        range::table_view<pcm_sample_t<F>> non_interleaved(
                m_read_buffer.data(),
                m_num_channels,
                m_period_size,
                1,
                m_num_channels);

        transform(non_interleaved, m_buffer.rows(), &pcm_convert::from<F>);

        return {};
    }

    void clear_buffer() noexcept override { fill(m_buffer.rows(), 0.f); }

private:
    system::device& m_fd;
    std::size_t m_num_channels;
    std::size_t m_period_size;
    std::vector<pcm_sample_t<F>> m_read_buffer;
    table m_buffer;
};

auto
make_reader(
        system::device& fd,
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

struct dummy_writer final : pcm_writer
{
    auto buffer() noexcept -> range::table_view<float> override { return {}; }

    auto write() noexcept -> std::error_code override { return {}; }
};

template <pcm_format F>
struct interleaved_writer final : pcm_writer
{
    interleaved_writer(
            system::device& fd,
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

    auto write() noexcept -> std::error_code override
    {
        range::table_view<pcm_sample_t<F>> non_interleaved(
                m_write_buffer.data(),
                m_num_channels,
                m_period_size,
                1,
                m_num_channels);
        transform(m_buffer.rows(), non_interleaved, &pcm_convert::to<F>);
        return writei(
                m_fd,
                m_write_buffer.data(),
                m_period_size,
                m_num_channels);
    }

private:
    system::device& m_fd;
    std::size_t m_num_channels;
    std::size_t m_period_size;
    std::vector<pcm_sample_t<F>> m_write_buffer;
    table m_buffer;
};

auto
make_writer(
        system::device& fd,
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
        system::device& input_fd,
        system::device& output_fd,
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

auto
process_step::operator()() -> std::error_condition
{
    if (m_starting)
    {
        if (m_output_fd)
        {
            fill(m_writer->buffer(), 0.f);

            for (unsigned i = 0; i < m_io_config.process_config.period_count;
                 ++i)
            {
                if (auto err = m_writer->write())
                    return err.default_error_condition();
            }
        }
        else if (m_input_fd)
        {
            if (auto err = m_input_fd.ioctl(SNDRV_PCM_IOCTL_START))
                return err.default_error_condition();
        }

        m_reader->clear_buffer();

        m_starting = false;
    }

    auto err = m_reader->read();

    cpu_load_meter cpu_load_meter(
            std::max(
                    m_reader->buffer().minor_size(),
                    m_writer->buffer().minor_size()),
            m_io_config.process_config.samplerate);

    m_process_function(m_reader->buffer(), m_writer->buffer());

    m_cpu_load.store(cpu_load_meter.stop(), std::memory_order_relaxed);

    if (!err)
        err = m_writer->write();

    if (err)
    {
        if (err == std::make_error_code(std::errc::broken_pipe))
        {
            system::device& fd = m_input_fd ? m_input_fd : m_output_fd;

            if (auto err = fd.ioctl(SNDRV_PCM_IOCTL_PREPARE))
                return err.default_error_condition();

            m_starting = true;
            ++m_xruns;
        }
        else
            return err.default_error_condition();
    }

    return {};
}

} // namespace piejam::audio::alsa
