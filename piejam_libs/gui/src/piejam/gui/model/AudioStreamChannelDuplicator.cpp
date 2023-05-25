// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023 Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioStreamChannelDuplicator.h>

#include <piejam/audio/multichannel_buffer.h>

namespace piejam::gui::model
{

void
AudioStreamChannelDuplicator::update(AudioStream const& stream)
{
    auto const buffer = duplicate_channels(stream);
    duplicated(buffer.view());
}

} // namespace piejam::gui::model
