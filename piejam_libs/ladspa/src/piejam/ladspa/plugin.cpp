// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/ladspa/plugin.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/ladspa/plugin_descriptor.h>
#include <piejam/ladspa/port_descriptor.h>
#include <piejam/npos.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>
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

namespace piejam::ladspa
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
        BOOST_ASSERT_MSG(
                !LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor),
                "not supported");

        if (control_input && !LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor))
        {
            spdlog::error("Lower bound not specified on int control input.");
        }

        int const min =
                LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)
                        ? boost::numeric_cast<int>(std::round(hint.LowerBound))
                        : std::numeric_limits<int>::min();

        if (control_input && !LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor))
        {
            spdlog::error("Upper bound not specified on int control input.");
        }

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
        BOOST_ASSERT_MSG(
                !LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor),
                "not supported");

        if (control_input && !LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor))
        {
            spdlog::error("Lower bound not specified on float control input.");
        }

        float const min = LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)
                                  ? hint.LowerBound
                                  : std::numeric_limits<float>::lowest();

        if (control_input && !LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor))
        {
            spdlog::error("Upper bound not specified on float control input.");
        }

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

        bool const logarithmic =
                static_cast<bool>(LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor));

        return float_port{
                .min = min,
                .max = max,
                .default_value = default_value,
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
        {
            m_descriptor.activate(m_handle);
        }
    }

    void deactivate() const noexcept
    {
        if (m_descriptor.deactivate)
        {
            m_descriptor.deactivate(m_handle);
        }
    }

    void connect_port(unsigned long port, LADSPA_Data* data) const noexcept
    {
        m_descriptor.connect_port(m_handle, port, data);
    }

    void run(unsigned long num_samples) const noexcept
    {
        m_descriptor.run(m_handle, num_samples);
    }

    void cleanup() const noexcept
    {
        m_descriptor.cleanup(m_handle);
    }

private:
    LADSPA_Descriptor const& m_descriptor;
    LADSPA_Handle m_handle;
};

struct to_event_port
{
    std::string_view name;

    auto operator()(float_port const&) const -> audio::engine::event_port
    {
        return audio::engine::event_port(std::in_place_type<float>, name);
    }

    auto operator()(int_port const&) const -> audio::engine::event_port
    {
        return audio::engine::event_port(std::in_place_type<int>, name);
    }

    auto operator()(bool_port const&) const -> audio::engine::event_port
    {
        return audio::engine::event_port(std::in_place_type<bool>, name);
    }
};

auto
to_event_ports(std::span<port_descriptor const> descs)
{
    return algorithm::transform_to_vector(
            descs,
            [](auto const& desc) -> audio::engine::event_port {
                return std::visit(to_event_port{desc.name}, desc.type_desc);
            });
}

struct control_input
{
    template <class T>
    control_input(std::in_place_type_t<T>)
        : m_initialize(&initialize<T>)
        , m_advance(&advance<T>)
    {
    }

    auto offset() const -> std::size_t
    {
        return m_offset;
    }

    auto data() const -> float const&
    {
        return m_data;
    }

    void initialize(
            audio::engine::event_input_buffers const& ev_in_bufs,
            std::size_t buf_index)
    {
        m_initialize(*this, ev_in_bufs, buf_index);
    }

    void advance()
    {
        m_advance(*this);
    }

private:
    template <class T>
    using ev_it_t = typename audio::engine::event_buffer<T>::const_iterator;

    template <class T>
    using ev_it_pair_t = std::pair<ev_it_t<T>, ev_it_t<T>>;

    template <class T>
    static void initialize(
            control_input& ci,
            audio::engine::event_input_buffers const& ev_bufs,
            std::size_t buf_index)
    {
        auto const& ev_buf = ev_bufs.get<T>(buf_index);

        using ev_it_pair_t = ev_it_pair_t<T>;
        static_assert(sizeof(ev_it_pair_t) < sizeof(m_iterators));

        ev_it_pair_t& its = *std::construct_at<ev_it_pair_t>(
                reinterpret_cast<ev_it_pair_t*>(&ci.m_iterators),
                ev_buf.begin(),
                ev_buf.end());

        ci.m_offset = its.first == its.second ? npos : its.first->offset();
    }

    template <class T>
    static void advance(control_input& ci)
    {
        using ev_it_pair_t = ev_it_pair_t<T>;
        static_assert(sizeof(ev_it_pair_t) < sizeof(m_iterators));

        ev_it_pair_t& its = *reinterpret_cast<ev_it_pair_t*>(&ci.m_iterators);

        BOOST_ASSERT(its.first != its.second);
        ci.m_data = static_cast<float>(its.first->value());
        std::advance(its.first, 1);

        ci.m_offset = its.first == its.second ? npos : its.first->offset();
    }

    std::size_t m_offset{};
    float m_data{};

    std::aligned_storage_t<32> m_iterators;

    using initialize_t = void (*)(
            control_input&,
            audio::engine::event_input_buffers const&,
            std::size_t);
    using advance_t = void (*)(control_input&);

    initialize_t m_initialize{};
    advance_t m_advance{};
};

class processor final : public audio::engine::processor
{
public:
    processor(
            plugin_instance instance,
            std::string_view name,
            std::span<port_descriptor const> audio_inputs,
            std::span<port_descriptor const> audio_outputs,
            std::span<port_descriptor const> control_inputs,
            std::span<port_descriptor const> control_outputs)
        : m_instance(std::move(instance))
        , m_name(name)
        , m_input_port_indices(audio_inputs.size())
        , m_output_port_indices(audio_outputs.size())
        , m_event_inputs(to_event_ports(control_inputs))
        , m_event_outputs(to_event_ports(control_outputs))
        , m_constant_audio_inputs(audio_inputs.size())
    {
        std::ranges::transform(
                audio_inputs,
                m_input_port_indices.begin(),
                &port_descriptor::index);

        std::ranges::transform(
                audio_outputs,
                m_output_port_indices.begin(),
                &port_descriptor::index);

        m_control_inputs.reserve(control_inputs.size());
        for (auto const& pd : control_inputs)
        {
            std::visit(
                    [this](auto const& port) {
                        using value_type = decltype(port.default_value);
                        m_control_inputs.emplace_back(
                                std::in_place_type<value_type>);
                    },
                    pd.type_desc);
            m_instance.connect_port(
                    pd.index,
                    const_cast<LADSPA_Data*>(&m_control_inputs.back().data()));
        }

        m_control_outputs.reserve(control_outputs.size());
        for (auto const& pd : control_outputs)
        {
            m_instance.connect_port(
                    pd.index,
                    &m_control_outputs.emplace_back());
        }

        m_instance.activate();
    }

    ~processor()
    {
        m_instance.deactivate();
        m_instance.cleanup();
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "ladspa_fx";
    }

    auto name() const noexcept -> std::string_view override
    {
        return m_name;
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return m_input_port_indices.size();
    }

    auto num_outputs() const noexcept -> std::size_t override
    {
        return m_output_port_indices.size();
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        return m_event_inputs;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return m_event_outputs;
    }

    void process(audio::engine::process_context const& ctx) override
    {
        audio::engine::verify_process_context(*this, ctx);

        BOOST_ASSERT(ctx.event_inputs.size() == m_event_inputs.size());
        for (std::size_t i : range::indices(ctx.event_inputs))
        {
            m_control_inputs[i].initialize(ctx.event_inputs, i);
        }

        BOOST_ASSERT(m_input_port_indices.size() == ctx.inputs.size());
        BOOST_ASSERT(m_output_port_indices.size() == ctx.outputs.size());

        std::size_t offset{};
        while (offset < ctx.buffer_size)
        {
            std::size_t const min_offset = std::min(
                    std::ranges::min(
                            m_control_inputs,
                            std::less<>{},
                            &control_input::offset)
                            .offset(),
                    ctx.buffer_size);

            if (offset < min_offset)
            {
                auto const slice_size = min_offset - offset;

                for (std::size_t i : range::indices(ctx.inputs))
                {
                    auto sub =
                            subslice(ctx.inputs[i].get(), offset, slice_size);

                    if (sub.is_constant())
                    {
                        std::ranges::fill_n(
                                m_constant_audio_inputs[i].begin(),
                                slice_size,
                                sub.constant());
                        sub = audio::engine::audio_slice(std::span(
                                m_constant_audio_inputs[i].data(),
                                slice_size));
                    }

                    m_instance.connect_port(
                            m_input_port_indices[i],
                            const_cast<LADSPA_Data*>(sub.buffer().data()));
                }

                for (std::size_t i : range::indices(ctx.outputs))
                {
                    auto sub = ctx.outputs[i].data() + offset;
                    m_instance.connect_port(m_output_port_indices[i], sub);
                }

                m_instance.run(static_cast<unsigned long>(slice_size));
            }

            for (control_input& ci : m_control_inputs)
            {
                while (min_offset == ci.offset())
                {
                    ci.advance();
                }
            }

            offset = min_offset;
        }

        std::ranges::copy(ctx.outputs, ctx.results.begin());
    }

private:
    plugin_instance m_instance;
    std::string m_name;
    std::vector<unsigned long> m_input_port_indices{};
    std::vector<unsigned long> m_output_port_indices{};
    std::vector<audio::engine::event_port> m_event_inputs;
    std::vector<audio::engine::event_port> m_event_outputs;
    std::vector<std::array<float, audio::max_period_size.get()>>
            m_constant_audio_inputs;
    std::vector<control_input> m_control_inputs;
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
        for (unsigned long port : range::iota(m_ladspa_desc->PortCount))
        {
            int const ladspa_port_desc = m_ladspa_desc->PortDescriptors[port];

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

    auto make_processor(audio::sample_rate const sample_rate) const
            -> std::unique_ptr<audio::engine::processor> override
    {
        if (LADSPA_Handle handle = m_ladspa_desc->instantiate(
                    m_ladspa_desc,
                    sample_rate.get()))
        {
            return std::make_unique<processor>(
                    plugin_instance(*m_ladspa_desc, handle),
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
        {
            throw std::runtime_error("Could not get LADSPA descriptor.");
        }
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

} // namespace piejam::ladspa
