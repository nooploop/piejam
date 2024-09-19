// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "get_set_hw_params.h"

#include <piejam/algorithm/contains.h>
#include <piejam/audio/io_process_config.h>
#include <piejam/audio/pcm_format.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/sound_card_hw_params.h>
#include <piejam/system/device.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <iterator>

namespace piejam::audio::alsa
{

namespace
{

auto
test_interval_value(
        system::device& fd,
        snd_pcm_hw_params params,
        unsigned const ival_index,
        unsigned const value) -> bool
{
    params.rmask = (1u << ival_index);
    params.cmask = 0u;

    auto& ival =
            params.intervals[ival_index - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    ival.min = value;
    ival.max = value;
    ival.integer = 1;

    if (auto err = fd.ioctl(SNDRV_PCM_IOCTL_HW_REFINE, params))
    {
        if (err != std::make_error_code(std::errc::invalid_argument))
        {
            auto const message = err.message();
            spdlog::error("get_hw_params/test_interval_value: {}", message);
        }

        return false;
    }

    return true;
}

auto
test_interval_value(
        system::device& fd,
        snd_pcm_hw_params const& params,
        unsigned const ival_index)
{
    return [&fd, &params, ival_index](unsigned const value) -> bool {
        return test_interval_value(fd, params, ival_index, value);
    };
}

constexpr auto
mask_offset(unsigned const val) noexcept -> unsigned
{
    return val >> 5u;
}

constexpr auto
mask_bit(unsigned const i) noexcept -> unsigned
{
    return 1u << (i & 31u);
}

constexpr auto
test_mask_bit(
        snd_pcm_hw_params const& params,
        unsigned const mask,
        unsigned const bit) -> bool
{
    return params.masks[mask - SNDRV_PCM_HW_PARAM_FIRST_MASK]
                   .bits[mask_offset(bit)] &
           mask_bit(bit);
}

auto
test_mask_bit(snd_pcm_hw_params const& params, unsigned const mask)
{
    return [&params, mask](unsigned const bit) {
        return test_mask_bit(params, mask, bit);
    };
}

constexpr void
set_mask_bit(snd_pcm_hw_params& params, unsigned const mask, unsigned const bit)
{
    params.masks[mask - SNDRV_PCM_HW_PARAM_FIRST_MASK].bits[mask_offset(bit)] =
            mask_bit(bit);
    params.rmask |= (1u << mask);
}

constexpr auto
get_interval(snd_pcm_hw_params const& params, unsigned const ival_index)
        -> snd_interval const&
{
    return params.intervals[ival_index - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
}

constexpr void
set_interval_value(
        snd_pcm_hw_params& params,
        unsigned const ival_index,
        unsigned const value)
{
    snd_interval& ival =
            params.intervals[ival_index - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    ival.min = value;
    ival.max = value;
    ival.openmin = 0;
    ival.openmax = 0;
    ival.integer = 1;
    params.rmask |= (1u << ival_index);
}

auto
make_snd_pcm_hw_params_for_refine_any() -> snd_pcm_hw_params
{
    snd_pcm_hw_params hw_params{};

    hw_params.rmask = ~0u;
    std::ranges::for_each(hw_params.masks, [](snd_mask& m) {
        std::ranges::fill(m.bits, ~0u);
    });
    std::ranges::for_each(hw_params.intervals, [](snd_interval& i) {
        i.min = 0;
        i.max = ~0u;
    });

    return hw_params;
}

constexpr auto
alsa_to_pcm_format(unsigned alsa_format) -> pcm_format
{
    switch (alsa_format)
    {
        case SNDRV_PCM_FORMAT_S8:
            return pcm_format::s8;
        case SNDRV_PCM_FORMAT_U8:
            return pcm_format::u8;
        case SNDRV_PCM_FORMAT_S16_LE:
            return pcm_format::s16_le;
        case SNDRV_PCM_FORMAT_S16_BE:
            return pcm_format::s16_be;
        case SNDRV_PCM_FORMAT_U16_LE:
            return pcm_format::u16_le;
        case SNDRV_PCM_FORMAT_U16_BE:
            return pcm_format::u16_be;
        case SNDRV_PCM_FORMAT_S32_LE:
            return pcm_format::s32_le;
        case SNDRV_PCM_FORMAT_S32_BE:
            return pcm_format::s32_be;
        case SNDRV_PCM_FORMAT_U32_LE:
            return pcm_format::u32_le;
        case SNDRV_PCM_FORMAT_U32_BE:
            return pcm_format::u32_be;
        case SNDRV_PCM_FORMAT_S24_3LE:
            return pcm_format::s24_3le;
        case SNDRV_PCM_FORMAT_S24_3BE:
            return pcm_format::s24_3be;
        case SNDRV_PCM_FORMAT_U24_3LE:
            return pcm_format::u24_3le;
        case SNDRV_PCM_FORMAT_U24_3BE:
            return pcm_format::u24_3be;
        default:
            return pcm_format::unsupported;
    }
}

constexpr auto
pcm_to_alsa_format(pcm_format pf) -> unsigned
{
    switch (pf)
    {
        case pcm_format::s8:
            return SNDRV_PCM_FORMAT_S8;
        case pcm_format::u8:
            return SNDRV_PCM_FORMAT_U8;
        case pcm_format::s16_le:
            return SNDRV_PCM_FORMAT_S16_LE;
        case pcm_format::s16_be:
            return SNDRV_PCM_FORMAT_S16_BE;
        case pcm_format::u16_le:
            return SNDRV_PCM_FORMAT_U16_LE;
        case pcm_format::u16_be:
            return SNDRV_PCM_FORMAT_U16_BE;
        case pcm_format::s32_le:
            return SNDRV_PCM_FORMAT_S32_LE;
        case pcm_format::s32_be:
            return SNDRV_PCM_FORMAT_S32_BE;
        case pcm_format::u32_le:
            return SNDRV_PCM_FORMAT_U32_LE;
        case pcm_format::u32_be:
            return SNDRV_PCM_FORMAT_U32_BE;
        case pcm_format::s24_3le:
            return SNDRV_PCM_FORMAT_S24_3LE;
        case pcm_format::s24_3be:
            return SNDRV_PCM_FORMAT_S24_3BE;
        case pcm_format::u24_3le:
            return SNDRV_PCM_FORMAT_U24_3LE;
        case pcm_format::u24_3be:
            return SNDRV_PCM_FORMAT_U24_3BE;
        default:
            throw std::runtime_error("unsupported pcm format");
    }
}

} // namespace

auto
get_hw_params(
        sound_card_descriptor const& pcm,
        sample_rate const* const sample_rate,
        period_size const* const period_size) -> sound_card_hw_params
{
    sound_card_hw_params result;

    auto hw_params = make_snd_pcm_hw_params_for_refine_any();

    system::device fd(pcm.path);
    if (auto err = fd.ioctl(SNDRV_PCM_IOCTL_HW_REFINE, hw_params))
    {
        throw std::system_error(err);
    }

    result.interleaved =
            test_mask_bit(
                    hw_params,
                    SNDRV_PCM_HW_PARAM_ACCESS,
                    SNDRV_PCM_ACCESS_RW_NONINTERLEAVED)
                    ? false
            : test_mask_bit(
                      hw_params,
                      SNDRV_PCM_HW_PARAM_ACCESS,
                      SNDRV_PCM_ACCESS_RW_INTERLEAVED)
                    ? true
                    : throw std::runtime_error(
                              "rw access not supported, only mmap?");

    static constexpr std::array preferred_formats{
            SNDRV_PCM_FORMAT_S32_LE,
            SNDRV_PCM_FORMAT_U32_LE,
            SNDRV_PCM_FORMAT_S32_BE,
            SNDRV_PCM_FORMAT_U32_BE,
            SNDRV_PCM_FORMAT_S24_3LE,
            SNDRV_PCM_FORMAT_S24_3BE,
            SNDRV_PCM_FORMAT_U24_3LE,
            SNDRV_PCM_FORMAT_U24_3BE,
            SNDRV_PCM_FORMAT_S16_LE,
            SNDRV_PCM_FORMAT_U16_LE,
            SNDRV_PCM_FORMAT_S16_BE,
            SNDRV_PCM_FORMAT_U16_BE,
            SNDRV_PCM_FORMAT_S8,
            SNDRV_PCM_FORMAT_U8};

    auto it_format = std::ranges::find_if(
            preferred_formats,
            test_mask_bit(hw_params, SNDRV_PCM_HW_PARAM_FORMAT));
    result.format =
            it_format != preferred_formats.end()
                    ? alsa_to_pcm_format(static_cast<unsigned>(*it_format))
                    : pcm_format::unsupported;

    result.num_channels =
            get_interval(hw_params, SNDRV_PCM_HW_PARAM_CHANNELS).max;

    BOOST_ASSERT(result.sample_rates.empty());
    std::ranges::copy_if(
            preferred_sample_rates,
            std::back_inserter(result.sample_rates),
            test_interval_value(fd, hw_params, SNDRV_PCM_HW_PARAM_RATE),
            &audio::sample_rate::value);

    if (sample_rate && algorithm::contains(result.sample_rates, *sample_rate))
    {
        set_interval_value(
                hw_params,
                SNDRV_PCM_HW_PARAM_RATE,
                sample_rate->value());
    }

    BOOST_ASSERT(result.period_sizes.empty());
    std::ranges::copy_if(
            preferred_period_sizes,
            std::back_inserter(result.period_sizes),
            test_interval_value(fd, hw_params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE),
            &audio::period_size::value);

    if (period_size && algorithm::contains(result.period_sizes, *period_size))
    {
        set_interval_value(
                hw_params,
                SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
                period_size->value());
    }

    BOOST_ASSERT(result.period_counts.empty());
    std::ranges::copy_if(
            preferred_period_counts,
            std::back_inserter(result.period_counts),
            test_interval_value(fd, hw_params, SNDRV_PCM_HW_PARAM_PERIODS),
            &audio::period_count::value);

    return result;
}

void
set_hw_params(
        system::device& fd,
        pcm_device_config const& device_config,
        pcm_process_config const& process_config)
{
    auto hw_params = make_snd_pcm_hw_params_for_refine_any();

    if (auto err = fd.ioctl(SNDRV_PCM_IOCTL_HW_REFINE, hw_params))
    {
        throw std::system_error(err);
    }

    hw_params.cmask = 0;

    unsigned const interleaved_bit =
            device_config.interleaved ? SNDRV_PCM_ACCESS_RW_INTERLEAVED
                                      : SNDRV_PCM_ACCESS_RW_NONINTERLEAVED;
    set_mask_bit(hw_params, SNDRV_PCM_HW_PARAM_ACCESS, interleaved_bit);
    set_mask_bit(
            hw_params,
            SNDRV_PCM_HW_PARAM_FORMAT,
            pcm_to_alsa_format(device_config.format));
    set_interval_value(
            hw_params,
            SNDRV_PCM_HW_PARAM_CHANNELS,
            device_config.num_channels);
    set_interval_value(
            hw_params,
            SNDRV_PCM_HW_PARAM_RATE,
            process_config.sample_rate.value());
    set_interval_value(
            hw_params,
            SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
            process_config.period_size.value());
    set_interval_value(
            hw_params,
            SNDRV_PCM_HW_PARAM_PERIODS,
            process_config.period_count.value());

    if (auto err = fd.ioctl(SNDRV_PCM_IOCTL_HW_PARAMS, hw_params))
    {
        throw std::system_error(err);
    }
}

} // namespace piejam::audio::alsa
