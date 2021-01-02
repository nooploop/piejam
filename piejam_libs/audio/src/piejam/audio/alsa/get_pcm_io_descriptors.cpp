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

#include <piejam/audio/alsa/get_pcm_io_descriptors.h>

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

                try
                {
                    fd.ioctl(SNDRV_CTL_IOCTL_CARD_INFO, card_info);
                    cards.emplace_back(entry.path(), card_info);
                }
                catch (std::exception const& err)
                {
                    spdlog::error("{}", err.what());
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
        fd.ioctl(SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, device);

        if (device == -1)
            break;

        snd_pcm_info info{};
        info.card = scard.info.card;
        info.device = static_cast<unsigned>(device);
        info.subdevice = 0;
        info.stream = stream_type;

        try
        {
            fd.ioctl(SNDRV_CTL_IOCTL_PCM_INFO, info);
            devices.push_back(std::move(info));
        }
        catch (std::system_error const& err)
        {
            static const auto error_to_ignore =
                    std::make_error_code(std::errc::no_such_file_or_directory);
            if (err.code() != error_to_ignore)
                throw;
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
            return {std::move(path), std::move(name)};
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
