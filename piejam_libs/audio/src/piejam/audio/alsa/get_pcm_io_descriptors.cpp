// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "get_pcm_io_descriptors.h"

#include <piejam/system/device.h>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <algorithm>

namespace piejam::audio::alsa
{

namespace
{

struct soundcard
{
    std::filesystem::path ctl_path;
    snd_ctl_card_info info;
};

const std::filesystem::path s_sound_devices_dir("/dev/snd");

auto
soundcards() -> std::vector<soundcard>
{
    std::vector<soundcard> cards;

    if (std::filesystem::exists(s_sound_devices_dir))
    {
        for (auto const& entry :
             std::filesystem::directory_iterator(s_sound_devices_dir))
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
                    spdlog::error("soundcards: {}", err.message());
                }
                else
                {
                    cards.emplace_back(entry.path(), card_info);
                }
            }
        }
    }

    return cards;
}

auto
get_devices(soundcard const& scard, int stream_type)
        -> std::vector<snd_pcm_info>
{
    std::vector<snd_pcm_info> devices;

    system::device fd(scard.ctl_path);

    int device{-1};
    do
    {
        if (auto err = fd.ioctl(SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, device))
        {
            spdlog::error("get_devices: {}", err.message());
            break;
        }

        if (device == -1)
            break;

        snd_pcm_info info{};
        info.card = scard.info.card;
        info.device = static_cast<unsigned>(device);
        info.subdevice = 0;
        info.stream = stream_type;

        if (auto err = fd.ioctl(SNDRV_CTL_IOCTL_PCM_INFO, info))
        {
            static const auto error_to_ignore =
                    std::make_error_code(std::errc::no_such_file_or_directory);
            if (err != error_to_ignore)
                spdlog::error("get_devices: {}", err.message());
        }
        else
        {
            devices.push_back(std::move(info));
        }
    } while (device != -1);

    return devices;
}

auto
input_devices(soundcard const& scard) -> std::vector<snd_pcm_info>
{
    return get_devices(scard, SNDRV_PCM_STREAM_CAPTURE);
}

auto
output_devices(soundcard const& scard) -> std::vector<snd_pcm_info>
{
    return get_devices(scard, SNDRV_PCM_STREAM_PLAYBACK);
}

} // namespace

auto
get_pcm_io_descriptors() -> pcm_io_descriptors
{
    pcm_io_descriptors result;

    auto to_pcm_descriptor = [](soundcard const& sc, char stream_type) {
        return [&sc,
                stream_type](snd_pcm_info const& pcm_info) -> pcm_descriptor {
            auto path = fmt::format(
                    "/dev/snd/pcmC{}D{}{}",
                    pcm_info.card,
                    pcm_info.device,
                    stream_type);
            auto name = fmt::format("{} - {}", sc.info.name, pcm_info.name);
            return pcm_descriptor{
                    .name = std::move(name),
                    .path = std::move(path)};
        };
    };

    for (soundcard const& sc : soundcards())
    {
        std::ranges::transform(
                input_devices(sc),
                std::back_inserter(result.inputs),
                to_pcm_descriptor(sc, 'c'));
        std::ranges::transform(
                output_devices(sc),
                std::back_inserter(result.outputs),
                to_pcm_descriptor(sc, 'p'));
    }

    return result;
}

} // namespace piejam::audio::alsa
