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

#include <piejam/audio/ladspa/plugin.h>

#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/system/dll.h>

#include <ladspa.h>

#include <boost/numeric/conversion/cast.hpp>

#include <cmath>
#include <stdexcept>
#include <variant>
#include <vector>

namespace piejam::audio::ladspa
{

namespace
{

struct float_port
{
    float min{};
    float max{1.f};
    float default_value{};
    bool multiply_with_samplerate{};
    bool logarithmic{};
};

struct int_port
{
    int min{};
    int max{};
    int default_value{};
};

struct bool_port
{
    bool default_value{};
};

using port_type_descriptor = std::variant<float_port, int_port, bool_port>;

auto
to_port_type_descriptor(LADSPA_PortRangeHint const& hint)
        -> port_type_descriptor
{
    auto const hint_descriptor = hint.HintDescriptor;
    if (LADSPA_IS_HINT_TOGGLED(hint_descriptor))
    {
        bool default_value{};
        if (LADSPA_IS_HINT_HAS_DEFAULT(hint_descriptor) &&
            LADSPA_IS_HINT_DEFAULT_1(hint_descriptor))
        {
            default_value = true;
        }

        return bool_port{.default_value = default_value};
    }
    else if (LADSPA_IS_HINT_INTEGER(hint_descriptor))
    {
        if (!LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor))
            throw std::runtime_error(
                    "Logarithmic integer ports are not supported.");

        if (!LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor))
            throw std::runtime_error(
                    "Min bound is not provided for integer port.");
        int const min = boost::numeric_cast<int>(std::round(hint.LowerBound));

        if (!LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor))
            throw std::runtime_error(
                    "Max bound is not provided for integer port.");
        int const max = boost::numeric_cast<int>(std::round(hint.UpperBound));

        int default_value{};
        if (LADSPA_IS_HINT_HAS_DEFAULT(hint_descriptor))
        {
            if (LADSPA_IS_HINT_DEFAULT_0(hint_descriptor))
            {
                default_value = 0;
            }
            else if (LADSPA_IS_HINT_DEFAULT_1(hint_descriptor))
            {
                default_value = 1;
            }
            else if (LADSPA_IS_HINT_DEFAULT_100(hint_descriptor))
            {
                default_value = 100;
            }
            else if (LADSPA_IS_HINT_DEFAULT_440(hint_descriptor))
            {
                default_value = 440;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hint_descriptor))
            {
                default_value = min;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hint_descriptor))
            {
                default_value = max;
            }
            else if (LADSPA_IS_HINT_DEFAULT_LOW(hint_descriptor))
            {
                default_value = boost::numeric_cast<int>(std::round(
                        hint.LowerBound * 0.75f + hint.UpperBound * 0.25f));
            }
            else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hint_descriptor))
            {
                default_value = boost::numeric_cast<int>(std::round(
                        hint.LowerBound * 0.5f + hint.UpperBound * 0.5f));
            }
            else if (LADSPA_IS_HINT_DEFAULT_HIGH(hint_descriptor))
            {
                default_value = boost::numeric_cast<int>(std::round(
                        hint.LowerBound * 0.25f + hint.UpperBound * 0.75f));
            }

            default_value = std::clamp(default_value, min, max);

            return int_port{
                    .min = min,
                    .max = max,
                    .default_value = default_value};
        }
    }
    else
    {
        float const min = LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)
                                  ? hint.LowerBound
                                  : std::numeric_limits<float>::lowest();

        float const max = LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor)
                                  ? hint.UpperBound
                                  : std::numeric_limits<float>::max();

        float default_value{};
        if (LADSPA_IS_HINT_HAS_DEFAULT(hint_descriptor))
        {
            if (LADSPA_IS_HINT_DEFAULT_0(hint_descriptor))
            {
                default_value = 0.f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_1(hint_descriptor))
            {
                default_value = 1.f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_100(hint_descriptor))
            {
                default_value = 100.f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_440(hint_descriptor))
            {
                default_value = 440.f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hint_descriptor))
            {
                default_value = min;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hint_descriptor))
            {
                default_value = max;
            }
            else if (LADSPA_IS_HINT_DEFAULT_LOW(hint_descriptor))
            {
                default_value = LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)
                                        ? std::exp(
                                                  std::log(min) * 0.75f +
                                                  std::log(max) * 0.25f)
                                        : min * 0.75f + max * 0.25f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hint_descriptor))
            {
                default_value = LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)
                                        ? std::exp(
                                                  std::log(min) * 0.5f +
                                                  std::log(max) * 0.5f)
                                        : min * 0.5f + max * 0.5f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_HIGH(hint_descriptor))
            {
                default_value = LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)
                                        ? std::exp(
                                                  std::log(min) * 0.25f +
                                                  std::log(max) * 0.75f)
                                        : min * 0.25f + max * 0.75f;
            }
        }

        default_value = std::clamp(default_value, min, max);

        return float_port{
                .min = min,
                .max = max,
                .default_value = default_value,
                .multiply_with_samplerate = static_cast<bool>(
                        LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)),
                .logarithmic = static_cast<bool>(
                        LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor))};
    }

    throw std::runtime_error("Could not retrieve port type descriptor.");
}

struct port_descriptor
{
    unsigned long index{};
    std::string name;
    port_type_descriptor type_desc;
};

class plugin_impl final : public plugin
{
public:
    plugin_impl(plugin_descriptor const& pd)
        : m_pd(pd)
        , m_plugin_dll(pd.file)
        , m_ladspa_desc(
                  throw_if_null(m_plugin_dll.symbol<LADSPA_Descriptor_Function>(
                          "ladspa_descriptor")(pd.index)))
    {
        for (unsigned long port = 0; port < m_ladspa_desc->PortCount; ++port)
        {
            auto const ladspa_port_desc = m_ladspa_desc->PortDescriptors[port];

            auto port_desc = port_descriptor{
                    .index = port,
                    .name = m_ladspa_desc->PortNames[port],
                    .type_desc = to_port_type_descriptor(
                            m_ladspa_desc->PortRangeHints[port])};

            if (LADSPA_IS_PORT_INPUT(ladspa_port_desc))
            {
                if (LADSPA_IS_PORT_AUDIO(ladspa_port_desc))
                {
                    m_ports.input.audio.push_back(std::move(port_desc));
                }
                else if (LADSPA_IS_PORT_CONTROL(ladspa_port_desc))
                {
                    m_ports.input.control.push_back(std::move(port_desc));
                }
            }
            else if (LADSPA_IS_PORT_OUTPUT(ladspa_port_desc))
            {
                if (LADSPA_IS_PORT_AUDIO(ladspa_port_desc))
                {
                    m_ports.output.audio.push_back(std::move(port_desc));
                }
                else if (LADSPA_IS_PORT_CONTROL(ladspa_port_desc))
                {
                    m_ports.output.control.push_back(std::move(port_desc));
                }
            }
        }
    }

    auto descriptor() const -> plugin_descriptor const& override
    {
        return m_pd;
    }

private:
    static auto throw_if_null(LADSPA_Descriptor const* d)
            -> LADSPA_Descriptor const*
    {
        if (!d)
            throw std::runtime_error("Could not get LADSPA descriptor.");
        return d;
    }

    static auto throw_if_null(LADSPA_Handle h) -> LADSPA_Handle
    {
        if (!h)
            throw std::runtime_error("Could not instantiate LADSPA plugin.");
        return h;
    }

    plugin_descriptor m_pd;
    system::dll m_plugin_dll;
    LADSPA_Descriptor const* m_ladspa_desc;

    struct
    {
        struct
        {
            std::vector<port_descriptor> audio;
            std::vector<port_descriptor> control;
        } input;

        struct
        {
            std::vector<port_descriptor> audio;
            std::vector<port_descriptor> control;
        } output;
    } m_ports{};
};

} // namespace

auto
load(plugin_descriptor const& pd) -> std::unique_ptr<plugin>
{
    return std::make_unique<plugin_impl>(pd);
}

} // namespace piejam::audio::ladspa
