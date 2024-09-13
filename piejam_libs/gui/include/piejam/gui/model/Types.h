// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <piejam/audio/types.h>

namespace piejam::gui::model
{

Q_NAMESPACE

enum class BusType : bool
{
    Mono,
    Stereo
};

Q_ENUM_NS(BusType)

[[nodiscard]] constexpr auto
toBusType(audio::bus_type bt) -> BusType
{
    switch (bt)
    {
        case audio::bus_type::mono:
            return BusType::Mono;
        case audio::bus_type::stereo:
            return BusType::Stereo;
    }
}

[[nodiscard]] constexpr auto
toBusType(BusType bt) -> audio::bus_type
{
    switch (bt)
    {
        case BusType::Mono:
            return audio::bus_type::mono;
        case BusType::Stereo:
            return audio::bus_type::stereo;
    }
}

template <class T>
[[nodiscard]] constexpr auto
busTypeTo(BusType const b, T&& mono_value, T&& stereo_value) -> decltype(auto)
{
    switch (b)
    {
        case BusType::Mono:
            return std::forward<T>(mono_value);
        case BusType::Stereo:
            return std::forward<T>(stereo_value);
    }
}

enum class DFTResolution
{
    Low,
    Medium,
    High,
    VeryHigh,
};

Q_ENUM_NS(DFTResolution)

enum class StereoChannel
{
    Left,
    Right,
    Middle,
    Side
};

Q_ENUM_NS(StereoChannel)

template <StereoChannel SC>
struct StereoFrameValue
{
    constexpr auto operator()(auto const frame) const noexcept -> float
    {
        switch (SC)
        {
            case StereoChannel::Left:
                return frame[0];
            case StereoChannel::Right:
                return frame[1];
            case StereoChannel::Middle:
                return frame[0] + frame[1];
            case StereoChannel::Side:
                return frame[0] - frame[1];
        }
    }
};

template <StereoChannel SC>
static constexpr StereoFrameValue<SC> stereoFrameValue;

enum class TriggerSlope
{
    RisingEdge,
    FallingEdge
};

Q_ENUM_NS(TriggerSlope)

} // namespace piejam::gui::model
