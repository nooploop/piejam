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

#include <piejam/audio/ladspa/scan.h>

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/system/dll.h>

#include <ladspa.h>

#include <boost/assert.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#include <system_error>

namespace piejam::audio::ladspa
{

auto
scan_file(std::filesystem::path const& file) -> std::vector<plugin_descriptor>
{
    BOOST_ASSERT(std::filesystem::exists(file));
    BOOST_ASSERT(std::filesystem::is_regular_file(file));

    std::vector<plugin_descriptor> result;

    system::dll plugin(file);
    auto const ladspa_descriptor =
            plugin.symbol<LADSPA_Descriptor_Function>("ladspa_descriptor");

    for (unsigned long plugin_index{};; ++plugin_index)
    {
        if (auto const desc = ladspa_descriptor(plugin_index))
        {
            std::size_t num_inputs{};
            std::size_t num_outputs{};
            for (unsigned long port = 0; port < desc->PortCount; ++port)
            {
                auto const port_desc = desc->PortDescriptors[port];
                if (LADSPA_IS_PORT_INPUT(port_desc))
                {
                    num_inputs +=
                            static_cast<bool>(LADSPA_IS_PORT_AUDIO(port_desc));
                }
                else if (LADSPA_IS_PORT_OUTPUT(port_desc))
                {
                    num_outputs +=
                            static_cast<bool>(LADSPA_IS_PORT_AUDIO(port_desc));
                }
            }

            result.push_back({
                    .file = file,
                    .index = plugin_index,
                    .id = desc->UniqueID,
                    .label = desc->Label,
                    .name = desc->Name,
                    .author = desc->Maker,
                    .copyright = desc->Copyright,
                    .num_inputs = num_inputs,
                    .num_outputs = num_outputs,
            });
        }
        else
            break;
    }

    return result;
}

auto
scan_directory(std::filesystem::path const& dir)
        -> std::vector<plugin_descriptor>
{
    BOOST_ASSERT(std::filesystem::exists(dir));
    BOOST_ASSERT(std::filesystem::is_directory(dir));

    std::vector<plugin_descriptor> result;

    for (auto const& entry : std::filesystem::directory_iterator(dir))
    {
        if (entry.is_regular_file() &&
            entry.path().extension().string() == ".so")
        {
            try
            {
                boost::push_back(result, scan_file(entry.path()));
            }
            catch (std::system_error const&)
            {
            }
        }
    }

    return result;
}

} // namespace piejam::audio::ladspa
