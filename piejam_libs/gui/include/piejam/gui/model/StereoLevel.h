// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>

#include <QObject>

#include <boost/assert.hpp>

namespace piejam::gui::model
{

class StereoLevel : public QObject
{
    Q_OBJECT

    M_PIEJAM_GUI_PROPERTY(double, levelLeft, setLevelLeft)
    M_PIEJAM_GUI_PROPERTY(double, levelRight, setLevelRight)

public:
    void setLevel(std::size_t channelIndex, double level)
    {
        switch (channelIndex)
        {
            case 0:
                setLevelLeft(level);
                break;

            case 1:
                setLevelRight(level);
                break;

            default:
                BOOST_ASSERT(false);
        }
    }

    template <std::size_t ChannelIndex>
        requires(ChannelIndex < 2)
    void setLevel(double level)
    {
        ChannelIndex == 0 ? setLevelLeft(level) : setLevelRight(level);
    }
};

} // namespace piejam::gui::model
