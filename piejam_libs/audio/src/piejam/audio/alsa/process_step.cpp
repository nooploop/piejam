// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "process_step.h"

#include "pcm_reader.h"
#include "pcm_writer.h"

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/cpu_load_meter.h>
#include <piejam/audio/io_process_config.h>
#include <piejam/audio/pcm_convert.h>
#include <piejam/audio/pcm_format.h>
#include <piejam/audio/pcm_sample_type.h>
#include <piejam/audio/types.h>
#include <piejam/numeric/rolling_mean.h>
#include <piejam/range/iota.h>
#include <piejam/range/strided_span.h>
#include <piejam/system/device.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <boost/assert.hpp>

#include <algorithm>

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
        {
            return err;
        }

        frames_transferred += static_cast<std::size_t>(arg.result);
        BOOST_ASSERT(frames_transferred <= frames);
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
    [[nodiscard]]
    auto converter() const noexcept -> std::span<converter_f const> override
    {
        return {};
    }

    auto transfer() noexcept -> std::error_code override
    {
        return {};
    }

    void clear() noexcept override
    {
    }
};

template <pcm_format F>
struct interleaved_reader final : pcm_reader
{
    interleaved_reader(
            system::device& fd,
            std::size_t const num_channels,
            audio::period_size const period_size)
        : m_fd(fd)
        , m_num_channels(num_channels)
        , m_period_size(period_size)
        , m_read_buffer(num_channels * period_size.value())
        , m_converter(algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [this](std::size_t const channel) {
                      return pcm_input_buffer_converter(
                              [this, channel](std::span<float> const buffer) {
                                  convert(channel, buffer);
                              });
                  }))
    {
        BOOST_ASSERT(m_fd);
    }

    void
    convert(std::size_t const channel, std::span<float> const buffer) noexcept
    {
        BOOST_ASSERT(channel < m_num_channels);
        BOOST_ASSERT(m_period_size.value() == buffer.size());

        range::strided_span<pcm_sample_t<F>> interleaved{
                m_read_buffer.data() + channel,
                buffer.size(),
                static_cast<std::ptrdiff_t>(m_num_channels)};

        std::ranges::transform(
                interleaved,
                buffer.begin(),
                &pcm_convert::from<F>);
    }

    [[nodiscard]]
    auto converter() const noexcept -> std::span<converter_f const> override
    {
        return m_converter;
    }

    auto transfer() noexcept -> std::error_code override
    {
        if (auto err =
                    readi(m_fd,
                          m_read_buffer.data(),
                          m_period_size.value(),
                          m_num_channels))
        {
            return err;
        }

        return {};
    }

    void clear() noexcept override
    {
        std::ranges::fill(m_read_buffer, pcm_sample_t<F>{});
    }

private:
    system::device& m_fd;
    std::size_t m_num_channels;
    period_size m_period_size;
    std::vector<pcm_sample_t<F>> m_read_buffer;
    std::vector<converter_f> m_converter;
};

auto
make_reader(
        system::device& fd,
        sound_card_config const& config,
        audio::period_size const period_size) -> std::unique_ptr<pcm_reader>
{
    if (!fd)
    {
        return std::make_unique<dummy_reader>();
    }

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
                BOOST_ASSERT(false);
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
    [[nodiscard]]
    auto converter() const noexcept -> std::span<converter_f const> override
    {
        return {};
    }

    auto transfer() noexcept -> std::error_code override
    {
        return {};
    }

    void clear() noexcept override
    {
    }
};

template <pcm_format F>
struct interleaved_writer final : pcm_writer
{
    interleaved_writer(
            system::device& fd,
            std::size_t const num_channels,
            audio::period_size const period_size)
        : m_fd(fd)
        , m_num_channels(num_channels)
        , m_period_size(period_size)
        , m_write_buffer(num_channels * period_size.value())
        , m_converter(algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [this](std::size_t const channel) {
                      return pcm_output_buffer_converter(
                              [this,
                               channel](float constant, std::size_t size) {
                                  convert(constant, size, channel);
                              },
                              [this,
                               channel](std::span<float const> source_buffer) {
                                  convert(source_buffer, channel);
                              });
                  }))
    {
        BOOST_ASSERT(m_fd);
    }

    void convert(std::span<float const> buffer, std::size_t channel)
    {
        BOOST_ASSERT(channel < m_num_channels);
        BOOST_ASSERT(m_period_size.value() == buffer.size());

        range::strided_span<pcm_sample_t<F>> interleaved{
                m_write_buffer.data() + channel,
                buffer.size(),
                static_cast<std::ptrdiff_t>(m_num_channels)};

        std::ranges::transform(
                buffer,
                interleaved.begin(),
                &pcm_convert::to<F>);
    }

    void convert(float constant, std::size_t size, std::size_t channel)
    {
        BOOST_ASSERT(channel < m_num_channels);

        range::strided_span<pcm_sample_t<F>> interleaved{
                m_write_buffer.data() + channel,
                size,
                static_cast<std::ptrdiff_t>(m_num_channels)};

        std::ranges::fill_n(
                interleaved.begin(),
                size,
                pcm_convert::to<F>(constant));
    }

    [[nodiscard]]
    auto converter() const noexcept -> std::span<converter_f const> override
    {
        return m_converter;
    }

    auto transfer() noexcept -> std::error_code override
    {
        return writei(
                m_fd,
                m_write_buffer.data(),
                m_period_size.value(),
                m_num_channels);
    }

    void clear() noexcept override
    {
        std::ranges::fill(m_write_buffer, pcm_sample_t<F>{});
    }

private:
    system::device& m_fd;
    std::size_t m_num_channels;
    period_size m_period_size;
    std::vector<pcm_sample_t<F>> m_write_buffer;
    std::vector<converter_f> m_converter;
};

auto
make_writer(
        system::device& fd,
        sound_card_config const& config,
        audio::period_size const period_size) -> std::unique_ptr<pcm_writer>
{
    if (!fd)
    {
        return std::make_unique<dummy_writer>();
    }

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
                BOOST_ASSERT(false);
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
        io_process_config const& io_config,
        std::atomic<float>& cpu_load,
        std::atomic_size_t& xruns,
        init_process_function const& init_process_function,
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
              m_io_config.buffer_config.period_size))
    , m_writer(make_writer(
              m_output_fd,
              m_io_config.out_config,
              m_io_config.buffer_config.period_size))
    , m_cpu_load_mean_acc(
              io_config.buffer_config.sample_rate.to_samples(
                      std::chrono::seconds{1}) /
              io_config.buffer_config.period_size.value()) // 1sec window
{
    m_xruns.store(0, std::memory_order_relaxed);

    init_process_function(m_reader->converter(), m_writer->converter());
}

process_step::process_step(process_step&&) = default;

process_step::~process_step() = default;

auto
process_step::operator()() -> std::error_condition
{
    if (m_starting)
    {
        system::device& fd = m_input_fd ? m_input_fd : m_output_fd;

        if (auto err = fd.ioctl(SNDRV_PCM_IOCTL_PREPARE))
        {
            return err.default_error_condition();
        }

        if (m_output_fd)
        {
            m_writer->clear();

            for (unsigned i = 0;
                 i < m_io_config.buffer_config.period_count.value();
                 ++i)
            {
                if (auto err = m_writer->transfer())
                {
                    return err.default_error_condition();
                }
            }
        }
        else if (m_input_fd)
        {
            if (auto err = m_input_fd.ioctl(SNDRV_PCM_IOCTL_START))
            {
                return err.default_error_condition();
            }
        }

        m_reader->clear();

        m_starting = false;
    }

    auto err = m_reader->transfer();

    if (!err)
    {
        cpu_load_meter cpu_load_meter(
                m_io_config.buffer_config.period_size.value(),
                m_io_config.buffer_config.sample_rate);

        m_process_function(m_io_config.buffer_config.period_size.value());

        m_cpu_load.store(
                m_cpu_load_mean_acc(cpu_load_meter.stop()),
                std::memory_order_relaxed);

        err = m_writer->transfer();
    }

    if (err)
    {
        if (err == std::make_error_code(std::errc::broken_pipe))
        {
            m_starting = true;
            ++m_xruns;
        }
        else
        {
            return err.default_error_condition();
        }
    }

    return {};
}

} // namespace piejam::audio::alsa
