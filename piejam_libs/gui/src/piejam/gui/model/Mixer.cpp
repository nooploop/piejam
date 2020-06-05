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

#include <piejam/gui/model/Mixer.h>

#include <algorithm>
#include <cassert>

namespace piejam::gui::model
{

auto
Mixer::inputChannels() -> QQmlListProperty<MixerChannel>
{
    return QQmlListProperty<MixerChannel>(
            this,
            nullptr,
            [](QQmlListProperty<MixerChannel>* property) -> int {
                Mixer* const mixer = qobject_cast<Mixer*>(property->object);
                assert(mixer);
                return static_cast<int>(mixer->m_inputChannels.size());
            },
            [](QQmlListProperty<MixerChannel>* property,
               int index) -> MixerChannel* {
                Mixer* const mixer = qobject_cast<Mixer*>(property->object);
                assert(mixer);
                return &mixer->m_inputChannels[static_cast<std::size_t>(index)];
            });
}

void
Mixer::setNumInputChannels(std::size_t const size)
{
    std::vector<MixerChannel> newInputChannels(size);
    std::swap(m_inputChannels, newInputChannels);
    emit inputChannelsChanged();
}

} // namespace piejam::gui::model
