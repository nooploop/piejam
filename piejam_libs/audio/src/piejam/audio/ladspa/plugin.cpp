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

#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/ladspa/plugin_descriptor.h>
#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/system/dll.h>

#include <ladspa.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <variant>
#include <vector>

namespace piejam::audio::ladspa
{

namespace
{

auto
to_port_type_descriptor(LADSPA_PortRangeHint const& hint, bool control_input)
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
        BOOST_ASSERT(!LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor));

        if (control_input && !LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor))
            spdlog::error("Lower bound not specified on int control input.");
        int const min =
                LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)
                        ? boost::numeric_cast<int>(std::round(hint.LowerBound))
                        : std::numeric_limits<int>::min();

        if (control_input && !LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor))
            spdlog::error("Upper bound not specified on int control input.");
        int const max =
                LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor)
                        ? boost::numeric_cast<int>(std::round(hint.UpperBound))
                        : std::numeric_limits<int>::max();

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
                default_value =
                        LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)
                                ? boost::numeric_cast<int>(std::round(std::exp(
                                          std::log(hint.LowerBound) * 0.75f +
                                          std::log(hint.UpperBound) * 0.25f)))
                                : boost::numeric_cast<int>(std::round(
                                          hint.LowerBound * 0.75f +
                                          hint.UpperBound * 0.25f));
            }
            else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hint_descriptor))
            {
                default_value =
                        LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)
                                ? boost::numeric_cast<int>(std::round(std::exp(
                                          std::log(hint.LowerBound) * 0.5f +
                                          std::log(hint.UpperBound) * 0.5f)))
                                : boost::numeric_cast<int>(std::round(
                                          hint.LowerBound * 0.5f +
                                          hint.UpperBound * 0.5f));
            }
            else if (LADSPA_IS_HINT_DEFAULT_HIGH(hint_descriptor))
            {
                default_value =
                        LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)
                                ? boost::numeric_cast<int>(std::round(std::exp(
                                          std::log(hint.LowerBound) * 0.25f +
                                          std::log(hint.UpperBound) * 0.75f)))
                                : boost::numeric_cast<int>(std::round(
                                          hint.LowerBound * 0.25f +
                                          hint.UpperBound * 0.75f));
            }
        }

        default_value = std::clamp(default_value, min, max);

        return int_port{
                .min = min,
                .max = max,
                .default_value = default_value,
                .logarithmic = static_cast<bool>(
                        LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor))};
    }
    else
    {
        if (control_input && !LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor))
            spdlog::error("Lower bound not specified on float control input.");
        float const min = LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)
                                  ? hint.LowerBound
                                  : std::numeric_limits<float>::lowest();

        if (control_input && !LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor))
            spdlog::error("Upper bound not specified on float control input.");
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

        bool const multiply_with_samplerate =
                static_cast<bool>(LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor));
        bool const logarithmic =
                static_cast<bool>(LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor));

        return float_port{
                .min = min,
                .max = max,
                .default_value = default_value,
                .multiply_with_samplerate = multiply_with_samplerate,
                .logarithmic = logarithmic};
    }

    throw std::runtime_error("Could not retrieve port type descriptor.");
}

class plugin_instance
{
public:
    plugin_instance(LADSPA_Descriptor const& d, LADSPA_Handle h) noexcept
        : m_descriptor(d)
        , m_handle(h)
    {
    }

    void activate() const noexcept
    {
        if (m_descriptor.activate)
            m_descriptor.activate(m_handle);
    }

    void deactivate() const noexcept
    {
        if (m_descriptor.deactivate)
            m_descriptor.deactivate(m_handle);
    }

    void connect_port(unsigned long port, LADSPA_Data* data) const noexcept
    {
        m_descriptor.connect_port(m_handle, port, data);
    }

    void run(unsigned long num_samples) const noexcept
    {
        m_descriptor.run(m_handle, num_samples);
    }

    void cleanup() const noexcept { m_descriptor.cleanup(m_handle); }

private:
    LADSPA_Descriptor const& m_descriptor;
    LADSPA_Handle m_handle;
};

struct to_event_port
{
    std::string_view name;

    auto operator()(audio::ladspa::float_port const&) const
            -> engine::event_port
    {
        return engine::event_port(std::in_place_type<float>, name);
    }

    auto operator()(audio::ladspa::int_port const&) const -> engine::event_port
    {
        return engine::event_port(std::in_place_type<int>, name);
    }

    auto operator()(audio::ladspa::bool_port const&) const -> engine::event_port
    {
        return engine::event_port(std::in_place_type<bool>, name);
    }
};

auto
to_event_ports(std::span<audio::ladspa::port_descriptor const> descs)
{
    std::vector<engine::event_port> result;
    std::ranges::transform(
            descs,
            std::back_inserter(result),
            [](auto const& desc) {
                return std::visit(to_event_port{desc.name}, desc.type_desc);
            });
    return result;
}

class processor final : public engine::processor
{
public:
    processor(
            plugin_instance instance,
            samplerate_t samplerate,
            std::string_view name,
            std::span<audio::ladspa::port_descriptor const> audio_inputs,
            std::span<audio::ladspa::port_descriptor const> audio_outputs,
            std::span<audio::ladspa::port_descriptor const> control_inputs,
            std::span<audio::ladspa::port_descriptor const> control_outputs)
        : m_instance(std::move(instance))
        , m_samplerate(samplerate)
        , m_name(name)
        , m_input_port_indices(audio_inputs.size())
        , m_output_port_indices(audio_outputs.size())
        , m_event_inputs(to_event_ports(control_inputs))
        , m_event_outputs(to_event_ports(control_outputs))
    {
        std::ranges::transform(
                audio_inputs,
                m_input_port_indices.begin(),
                &audio::ladspa::port_descriptor::index);

        std::ranges::transform(
                audio_outputs,
                m_output_port_indices.begin(),
                &audio::ladspa::port_descriptor::index);

        m_control_inputs.reserve(control_inputs.size());
        for (auto const& pd : control_inputs)
        {
            if (std::holds_alternative<float_port>(pd.type_desc))
            {
                m_instance.connect_port(
                        pd.index,
                        &m_control_inputs
                                 .emplace_back(
                                         0.f,
                                         std::get<float_port>(pd.type_desc)
                                                 .multiply_with_samplerate)
                                 .value);
            }
            else
            {
                m_instance.connect_port(
                        pd.index,
                        &m_control_inputs.emplace_back().value);
            }
        }

        m_control_outputs.reserve(control_outputs.size());
        for (auto const& pd : control_outputs)
            m_instance.connect_port(
                    pd.index,
                    &m_control_outputs.emplace_back());

        m_instance.activate();
    }

    ~processor()
    {
        m_instance.deactivate();
        m_instance.cleanup();
    }

    auto type_name() const -> std::string_view override { return "ladspa_fx"; }

    auto name() const -> std::string_view override { return m_name; }

    auto num_inputs() const -> std::size_t override
    {
        return m_input_port_indices.size();
    }

    auto num_outputs() const -> std::size_t override
    {
        return m_output_port_indices.size();
    }

    auto event_inputs() const -> event_ports override { return m_event_inputs; }

    auto event_outputs() const -> event_ports override
    {
        return m_event_outputs;
    }

    void process(engine::process_context const& ctx) override
    {
        engine::verify_process_context(*this, ctx);

        BOOST_ASSERT(ctx.event_inputs.size() == m_event_inputs.size());
        for (std::size_t i = 0; i < ctx.event_inputs.size(); ++i)
        {
            auto const in_type = m_event_inputs[i].type();
            if (in_type == typeid(float))
            {
                auto const& ev_buf = ctx.event_inputs.get<float>(i);
                for (auto const& ev : ev_buf)
                {
                    m_control_inputs[i].value = ev.value();
                    if (m_control_inputs[i].multiply_with_samplerate)
                        m_control_inputs[i].value *= m_samplerate;
                }
            }
            else if (in_type == typeid(int))
            {
                auto const& ev_buf = ctx.event_inputs.get<int>(i);
                for (auto const& ev : ev_buf)
                {
                    m_control_inputs[i].value = static_cast<float>(ev.value());
                }
            }
            else if (in_type == typeid(bool))
            {
                auto const& ev_buf = ctx.event_inputs.get<bool>(i);
                for (auto const& ev : ev_buf)
                {
                    m_control_inputs[i].value = static_cast<float>(ev.value());
                }
            }
        }

        BOOST_ASSERT(m_input_port_indices.size() == ctx.inputs.size());
        for (std::size_t i = 0; i < ctx.inputs.size(); ++i)
        {
            m_instance.connect_port(
                    m_input_port_indices[i],
                    const_cast<LADSPA_Data*>(
                            ctx.inputs[i].get().buffer().data()));
        }

        BOOST_ASSERT(m_output_port_indices.size() == ctx.outputs.size());
        for (std::size_t i = 0; i < ctx.outputs.size(); ++i)
        {
            m_instance.connect_port(
                    m_output_port_indices[i],
                    ctx.outputs[i].data());
            ctx.results[i] = ctx.outputs[i];
        }

        m_instance.run(static_cast<unsigned long>(ctx.buffer_size));
    }

private:
    struct control_data
    {
        float value{};
        bool multiply_with_samplerate{};
    };

    plugin_instance m_instance;
    samplerate_t m_samplerate;
    std::string m_name;
    std::vector<unsigned long> m_input_port_indices{};
    std::vector<unsigned long> m_output_port_indices{};
    std::vector<engine::event_port> m_event_inputs;
    std::vector<engine::event_port> m_event_outputs;
    std::vector<control_data> m_control_inputs;
    std::vector<float> m_control_outputs;
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
                            m_ladspa_desc->PortRangeHints[port],
                            LADSPA_IS_PORT_INPUT(ladspa_port_desc) &&
                                    LADSPA_IS_PORT_CONTROL(ladspa_port_desc))};

            if (LADSPA_IS_PORT_INPUT(ladspa_port_desc))
            {
                if (LADSPA_IS_PORT_AUDIO(ladspa_port_desc))
                {
                    BOOST_ASSERT(std::holds_alternative<float_port>(
                            port_desc.type_desc));
                    m_ports.input.audio.push_back(std::move(port_desc));
                }
                else if (LADSPA_IS_PORT_CONTROL(ladspa_port_desc))
                {
                    m_ports.input.control.push_back(std::move(port_desc));
                }
                else
                {
                    throw std::runtime_error(
                            "Port is whether audio nor control.");
                }
            }
            else if (LADSPA_IS_PORT_OUTPUT(ladspa_port_desc))
            {
                if (LADSPA_IS_PORT_AUDIO(ladspa_port_desc))
                {
                    BOOST_ASSERT(std::holds_alternative<float_port>(
                            port_desc.type_desc));
                    m_ports.output.audio.push_back(std::move(port_desc));
                }
                else if (LADSPA_IS_PORT_CONTROL(ladspa_port_desc))
                {
                    m_ports.output.control.push_back(std::move(port_desc));
                }
                else
                {
                    throw std::runtime_error(
                            "Port is whether audio nor control.");
                }
            }
            else
            {
                throw std::runtime_error("Port is whether input nor output.");
            }
        }
    }

    auto descriptor() const -> plugin_descriptor const& override
    {
        return m_pd;
    }

    auto control_inputs() const -> std::span<port_descriptor const> override
    {
        return m_ports.input.control;
    }

    auto make_processor(samplerate_t samplerate) const
            -> std::unique_ptr<engine::processor> override
    {
        if (LADSPA_Handle handle =
                    m_ladspa_desc->instantiate(m_ladspa_desc, samplerate))
        {
            return std::make_unique<processor>(
                    plugin_instance(*m_ladspa_desc, handle),
                    samplerate,
                    m_pd.name,
                    m_ports.input.audio,
                    m_ports.output.audio,
                    m_ports.input.control,
                    m_ports.output.control);
        }

        return nullptr;
    }

private:
    static auto throw_if_null(LADSPA_Descriptor const* d)
            -> LADSPA_Descriptor const*
    {
        if (!d)
            throw std::runtime_error("Could not get LADSPA descriptor.");
        return d;
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
