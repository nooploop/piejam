// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <piejam/audio/types.h>

namespace piejam::gui::model
{

class BusTypeClass
{
    Q_GADGET

    BusTypeClass() noexcept = default;

public:
    enum class Value : bool
    {
        Mono,
        Stereo
    };

    Q_ENUM(Value)
};

using BusType = BusTypeClass::Value;

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

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::BusType)
