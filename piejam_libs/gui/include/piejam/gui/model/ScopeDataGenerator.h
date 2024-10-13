// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/ScopeData.h>
#include <piejam/gui/model/Types.h>

#include <boost/container/container_fwd.hpp>

namespace piejam::gui::model
{

class ScopeDataGenerator
{
public:
    using Streams = boost::container::static_vector<std::span<float const>, 2>;

    auto
    process(std::size_t triggerStream,
            Streams,
            std::size_t windowSize,
            TriggerSlope,
            float triggerLevel,
            std::size_t capturedFrames,
            std::size_t holdTimeInFrames) -> Streams;

    void clear()
    {
        m_state = ScopeDataGeneratorState::WaitingForTrigger;
        m_holdCapturedSize = 0;
    }

private:
    enum class ScopeDataGeneratorState : bool
    {
        WaitingForTrigger,
        Hold
    };

    ScopeDataGeneratorState m_state{};
    std::size_t m_holdCapturedSize{};
};

} // namespace piejam::gui::model
