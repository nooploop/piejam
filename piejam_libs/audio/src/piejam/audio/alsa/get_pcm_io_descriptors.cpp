// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "get_pcm_io_descriptors.h"

#include <piejam/system/device.h>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <filesystem>
#include <string_view>

namespace piejam::audio::alsa
{

namespace
{

struct soundcard
{
    std::filesystem::path ctl_path;
    snd_ctl_card_info info;
};

std::filesystem::path const s_sound_devices_dir("/dev/snd");

auto
soundcards() -> std::vector<soundcard>
{
    std::vector<soundcard> cards;

    std::error_code ec;
    if (std::filesystem::exists(s_sound_devices_dir, ec))
    {
        for (auto const& entry :
             std::filesystem::directory_iterator(s_sound_devices_dir, ec))
        {
            unsigned card{};
            if (1 == std::sscanf(
                             entry.path().filename().c_str(),
                             "controlC%u",
                             &card))
            {
                system::device fd(entry.path());
                snd_ctl_card_info card_info{};

                if (auto err = fd.ioctl(SNDRV_CTL_IOCTL_CARD_INFO, card_info))
                {
                    auto const message = err.message();
                    spdlog::error("soundcards: {}", message);
                }
                else
                {
                    cards.emplace_back(soundcard{
                            .ctl_path = entry.path(),
                            .info = card_info});
                }
            }
        }
    }

    return cards;
}

auto
get_devices(soundcard const& sc, int stream_type) -> std::vector<snd_pcm_info>
{
    BOOST_ASSERT(
            stream_type == SNDRV_PCM_STREAM_CAPTURE ||
            stream_type == SNDRV_PCM_STREAM_PLAYBACK);

    std::vector<snd_pcm_info> devices;

    system::device fd(sc.ctl_path);

    int device{-1};
    do
    {
        if (auto err = fd.ioctl(SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, device))
        {
            auto const message = err.message();
            spdlog::error("get_devices: {}", message);
            break;
        }

        if (device == -1)
        {
            break;
        }

        snd_pcm_info info{};
        info.card = sc.info.card;
        info.device = static_cast<unsigned>(device);
        info.subdevice = 0;
        info.stream = stream_type;

        if (auto err = fd.ioctl(SNDRV_CTL_IOCTL_PCM_INFO, info))
        {
            static auto const error_to_ignore =
                    std::make_error_code(std::errc::no_such_file_or_directory);
            if (err != error_to_ignore)
            {
                auto const message = err.message();
                spdlog::error("get_devices: {}", message);
            }
        }
        else
        {
            devices.emplace_back(info);
        }
    } while (device != -1);

    return devices;
}

auto
input_devices(soundcard const& sc) -> std::vector<snd_pcm_info>
{
    return get_devices(sc, SNDRV_PCM_STREAM_CAPTURE);
}

auto
output_devices(soundcard const& sc) -> std::vector<snd_pcm_info>
{
    return get_devices(sc, SNDRV_PCM_STREAM_PLAYBACK);
}

struct to_pcm_descriptor
{
    to_pcm_descriptor(soundcard const& sc, char stream_type)
        : sc{sc}
        , stream_type{stream_type}
    {
        BOOST_ASSERT(stream_type == 'c' || stream_type == 'p');
    }

    auto operator()(snd_pcm_info const& pcm_info) const -> pcm_descriptor
    {
        return pcm_descriptor{
                .name = fmt::format(
                        "{} - {}",
                        reinterpret_cast<char const*>(sc.info.name),
                        reinterpret_cast<char const*>(pcm_info.name)),
                .path = fmt::format(
                        "/dev/snd/pcmC{}D{}{}",
                        pcm_info.card,
                        pcm_info.device,
                        stream_type)};
    }

    soundcard const& sc;
    char stream_type;
};

} // namespace

auto
get_pcm_io_descriptors() -> pcm_io_descriptors
{
    pcm_io_descriptors result;

    for (soundcard const& sc : soundcards())
    {
        std::ranges::transform(
                input_devices(sc),
                std::back_inserter(result.inputs),
                to_pcm_descriptor{sc, 'c'});
        std::ranges::transform(
                output_devices(sc),
                std::back_inserter(result.outputs),
                to_pcm_descriptor{sc, 'p'});
    }

    return result;
}

} // namespace piejam::audio::alsa
