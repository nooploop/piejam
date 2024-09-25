// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include "get_io_sound_cards.h"

#include <piejam/box.h>
#include <piejam/io_pair.h>
#include <piejam/system/device.h>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <sound/asound.h>
#include <sys/ioctl.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <filesystem>

namespace piejam::audio::alsa
{

namespace
{

struct sound_card_info
{
    std::filesystem::path control_path;
    snd_ctl_card_info info;
};

std::filesystem::path const s_sound_cards_dir("/dev/snd");

auto
scan_for_sound_cards() -> std::vector<sound_card_info>
{
    std::vector<sound_card_info> cards;

    std::error_code ec;
    if (std::filesystem::exists(s_sound_cards_dir, ec))
    {
        for (auto const& entry :
             std::filesystem::directory_iterator(s_sound_cards_dir, ec))
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
                    spdlog::error("scan_for_sound_cards: {}", message);
                }
                else
                {
                    cards.emplace_back(sound_card_info{
                            .control_path = entry.path(),
                            .info = card_info});
                }
            }
        }
    }

    return cards;
}

auto
get_sound_card_pcm_infos(sound_card_info const& sc, int stream_type)
        -> std::vector<snd_pcm_info>
{
    BOOST_ASSERT(
            stream_type == SNDRV_PCM_STREAM_CAPTURE ||
            stream_type == SNDRV_PCM_STREAM_PLAYBACK);

    std::vector<snd_pcm_info> sound_card_pcm_infos;

    system::device fd(sc.control_path);

    int device{-1};
    do
    {
        if (auto err = fd.ioctl(SNDRV_CTL_IOCTL_PCM_NEXT_DEVICE, device))
        {
            auto const message = err.message();
            spdlog::error("get_sound_card_pcm_infos: {}", message);
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
                spdlog::error("get_sound_card_pcm_infos: {}", message);
            }
        }
        else
        {
            sound_card_pcm_infos.emplace_back(info);
        }
    } while (device != -1);

    return sound_card_pcm_infos;
}

auto
input_pcm_infos(sound_card_info const& sc) -> std::vector<snd_pcm_info>
{
    return get_sound_card_pcm_infos(sc, SNDRV_PCM_STREAM_CAPTURE);
}

auto
output_pcm_infos(sound_card_info const& sc) -> std::vector<snd_pcm_info>
{
    return get_sound_card_pcm_infos(sc, SNDRV_PCM_STREAM_PLAYBACK);
}

struct to_sound_card_descriptor
{
    to_sound_card_descriptor(sound_card_info const& sc, char stream_type)
        : sc{sc}
        , stream_type{stream_type}
    {
        BOOST_ASSERT(stream_type == 'c' || stream_type == 'p');
    }

    auto operator()(snd_pcm_info const& pcm_info) const -> sound_card_descriptor
    {
        return sound_card_descriptor{
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

    sound_card_info const& sc;
    char stream_type;
};

} // namespace

auto
get_io_sound_cards() -> io_sound_cards
{
    io_pair<std::vector<sound_card_descriptor>> result;

    for (sound_card_info const& sc_info : scan_for_sound_cards())
    {
        std::ranges::transform(
                input_pcm_infos(sc_info),
                std::back_inserter(result.in),
                to_sound_card_descriptor{sc_info, 'c'});
        std::ranges::transform(
                output_pcm_infos(sc_info),
                std::back_inserter(result.out),
                to_sound_card_descriptor{sc_info, 'p'});
    }

    return io_sound_cards{
            box(std::move(result.in)),
            box(std::move(result.out)),
    };
}

} // namespace piejam::audio::alsa
