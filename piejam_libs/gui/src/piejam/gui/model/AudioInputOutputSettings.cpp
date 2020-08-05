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

#include <piejam/gui/model/AudioInputOutputSettings.h>

#include <algorithm>
#include <cassert>

namespace piejam::gui::model
{

void
AudioInputOutputSettings::setChannels(QStringList const& channels)
{
    if (m_channels != channels)
    {
        m_channels = channels;
        emit channelsChanged();
    }
}

auto
AudioInputOutputSettings::busConfigs() -> QQmlListProperty<BusConfig>
{
    return QQmlListProperty<BusConfig>(
            this,
            nullptr,
            [](QQmlListProperty<BusConfig>* property) -> int {
                AudioInputOutputSettings* const settings =
                        qobject_cast<AudioInputOutputSettings*>(
                                property->object);
                assert(settings);
                return static_cast<int>(settings->m_busConfigs.size());
            },
            [](QQmlListProperty<BusConfig>* property, int index) -> BusConfig* {
                AudioInputOutputSettings* const settings =
                        qobject_cast<AudioInputOutputSettings*>(
                                property->object);
                assert(settings);
                return &settings->m_busConfigs[static_cast<std::size_t>(index)];
            });
}

void
AudioInputOutputSettings::setBusConfigs(std::vector<BusConfig> busConfigs)
{
    std::swap(m_busConfigs, busConfigs);
    emit busConfigsChanged();
}

} // namespace piejam::gui::model
