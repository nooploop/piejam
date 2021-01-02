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

#include <piejam/audio/alsa/get_set_hw_params.h>

#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_format.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/pcm_io_config.h>
#include <piejam/audio/period_sizes.h>
#include <piejam/audio/samplerates.h>
#include <piejam/system/device.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <iterator>

namespace piejam::audio::alsa
{

static auto
test_interval_value(
        system::device& fd,
        snd_pcm_hw_params params,
        unsigned ival_index,
        unsigned value) -> bool
{
    try
    {
        params.rmask = (1u << ival_index);
        params.cmask = 0u;
        auto& ival = params.intervals
                             [ival_index - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
        ival.min = value;
        ival.max = value;
        ival.integer = 1;
        fd.ioctl(SNDRV_PCM_IOCTL_HW_REFINE, params);
    }
    catch (std::system_error const& err)
    {
        if (err.code() != std::make_error_code(std::errc::invalid_argument))
            throw;
        return false;
    }

    return true;
}

static auto
test_interval_value(
        system::device& fd,
        snd_pcm_hw_params const& params,
        unsigned ival_index)
{
    return [&fd, &params, ival_index](unsigned samplerate) {
        return test_interval_value(fd, params, ival_index, samplerate);
    };
}

static constexpr unsigned
mask_offset(unsigned const val) noexcept
{
    return val >> 5u;
}

static constexpr unsigned
mask_bit(unsigned const i) noexcept
{
    return 1u << (i & 31u);
}

static constexpr bool
test_mask_bit(
        snd_pcm_hw_params const& params,
        unsigned const mask,
        unsigned const bit)
{
    return params.masks[mask - SNDRV_PCM_HW_PARAM_FIRST_MASK]
                   .bits[mask_offset(bit)] &
           mask_bit(bit);
}

static auto
test_mask_bit(snd_pcm_hw_params const& params, unsigned mask)
{
    return [&params, mask](unsigned const bit) {
        return test_mask_bit(params, mask, bit);
    };
}

static constexpr void
set_mask_bit(snd_pcm_hw_params& params, unsigned mask, unsigned bit)
{
    params.masks[mask - SNDRV_PCM_HW_PARAM_FIRST_MASK].bits[mask_offset(bit)] =
            mask_bit(bit);
    params.rmask |= (1u << mask);
}

static constexpr auto
get_interval(snd_pcm_hw_params const& params, unsigned const interval_index)
        -> snd_interval const&
{
    return params.intervals[interval_index - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
}

static constexpr void
set_interval_value(
        snd_pcm_hw_params& params,
        unsigned const interval_index,
        unsigned const value)
{
    snd_interval& ival =
            params.intervals
                    [interval_index - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    ival.min = value;
    ival.max = value;
    ival.openmin = 0;
    ival.openmax = 0;
    ival.integer = 1;
    params.rmask |= (1u << interval_index);
}

static auto
make_snd_pcm_hw_params_for_refine_any() -> snd_pcm_hw_params
{
    snd_pcm_hw_params hw_params{};

    hw_params.rmask = ~0u;
    std::for_each(
            std::begin(hw_params.masks),
            std::end(hw_params.masks),
            [](snd_mask& m) { std::ranges::fill(m.bits, ~0u); });
    std::for_each(
            std::begin(hw_params.intervals),
            std::end(hw_params.intervals),
            [](snd_interval& i) {
                i.min = 0;
                i.max = ~0u;
            });

    return hw_params;
}

static constexpr auto
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

static constexpr auto
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

auto
get_hw_params(pcm_descriptor const& pcm) -> pcm_hw_params
{
    pcm_hw_params result;

    if (pcm.path.empty())
    {
        std::ranges::copy(
                preferred_samplerates,
                std::back_inserter(result.samplerates));
        std::ranges::copy(
                preferred_period_sizes,
                std::back_inserter(result.period_sizes));
        return result;
    }

    auto hw_params = make_snd_pcm_hw_params_for_refine_any();

    system::device fd(pcm.path);
    fd.ioctl(SNDRV_PCM_IOCTL_HW_REFINE, hw_params);

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

    auto it_format = std::find_if(
            preferred_formats.begin(),
            preferred_formats.end(),
            test_mask_bit(hw_params, SNDRV_PCM_HW_PARAM_FORMAT));
    result.format =
            it_format != preferred_formats.end()
                    ? alsa_to_pcm_format(static_cast<unsigned>(*it_format))
                    : pcm_format::unsupported;

    result.num_channels =
            get_interval(hw_params, SNDRV_PCM_HW_PARAM_CHANNELS).max;

    assert(result.samplerates.empty());
    std::ranges::copy_if(
            preferred_samplerates,
            std::back_inserter(result.samplerates),
            test_interval_value(fd, hw_params, SNDRV_PCM_HW_PARAM_RATE));

    assert(result.period_sizes.empty());
    std::ranges::copy_if(
            preferred_period_sizes,
            std::back_inserter(result.period_sizes),
            test_interval_value(fd, hw_params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE));

    snd_interval const& period_count =
            get_interval(hw_params, SNDRV_PCM_HW_PARAM_PERIODS);
    result.period_count_min = period_count.min;
    result.period_count_max = period_count.max;

    return result;
}

void
set_hw_params(
        system::device& fd,
        pcm_device_config const& device_config,
        pcm_process_config const& process_config)
{
    auto hw_params = make_snd_pcm_hw_params_for_refine_any();
    fd.ioctl(SNDRV_PCM_IOCTL_HW_REFINE, hw_params);
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
            process_config.samplerate);
    set_interval_value(
            hw_params,
            SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
            process_config.period_size);
    set_interval_value(
            hw_params,
            SNDRV_PCM_HW_PARAM_PERIODS,
            process_config.period_count);

    fd.ioctl(SNDRV_PCM_IOCTL_HW_PARAMS, hw_params);
}

} // namespace piejam::audio::alsa
