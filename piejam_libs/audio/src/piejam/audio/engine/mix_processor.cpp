// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/mix_processor.h>

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/slice_algorithms.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/functional/compare.h>
#include <piejam/npos.h>

#include <boost/assert.hpp>
#include <boost/preprocessor/iteration/local.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

namespace
{

template <std::size_t N>
auto
mix(process_context const& ctx)
{
    return add(ctx.inputs[N - 1].get(), mix<N - 1>(ctx), ctx.outputs[0]);
}

template <>
auto
mix<2>(process_context const& ctx)
{
    return add(ctx.inputs[1].get(), ctx.inputs[0].get(), ctx.outputs[0]);
}

template <>
auto
mix<npos>(process_context const& ctx)
{
    audio_slice res;

    auto const out = ctx.outputs[0];

    for (audio_slice const& in : ctx.inputs)
    {
        if (is_silence(in))
            continue;

        res = add(in, res, out);
    }

    return res;
}

template <std::size_t NumInputs>
class mix_processor final : public named_processor
{
public:
    mix_processor(std::string_view const name) requires(NumInputs != npos)
        : named_processor(name)
        , m_num_inputs(NumInputs)
    {
    }

    mix_processor(
            std::size_t const num_inputs,
            std::string_view const name) requires(NumInputs == npos)
        : named_processor(name)
        , m_num_inputs(num_inputs)
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "mix";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return NumInputs != npos ? NumInputs : m_num_inputs;
    }
    auto num_outputs() const noexcept -> std::size_t override { return 1; }

    auto event_inputs() const noexcept -> event_ports override { return {}; }
    auto event_outputs() const noexcept -> event_ports override { return {}; }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        ctx.results[0] = mix<NumInputs>(ctx);
    }

private:
    std::size_t const m_num_inputs{};
};

} // namespace

#define PIEJAM_MAX_NUM_FIXED_INPUTS_MIX_PROCESSOR 8

auto
make_mix_processor(std::size_t const num_inputs, std::string_view const name)
        -> std::unique_ptr<processor>
{
    switch (num_inputs)
    {

#define BOOST_PP_LOCAL_LIMITS (2, PIEJAM_MAX_NUM_FIXED_INPUTS_MIX_PROCESSOR)
#define BOOST_PP_LOCAL_MACRO(n)                                                \
    case n:                                                                    \
        return std::make_unique<mix_processor<n>>(name);
#include BOOST_PP_LOCAL_ITERATE()

        default:
            BOOST_ASSERT(num_inputs > 1);
            return std::make_unique<mix_processor<npos>>(num_inputs, name);
    }
}

bool
is_mix_processor(processor const& proc) noexcept
{
    static std::array mix_processor_typeids{

#define BOOST_PP_LOCAL_LIMITS (2, PIEJAM_MAX_NUM_FIXED_INPUTS_MIX_PROCESSOR)
#define BOOST_PP_LOCAL_MACRO(n) std::type_index(typeid(mix_processor<n>)),
#include BOOST_PP_LOCAL_ITERATE()

            std::type_index(typeid(mix_processor<npos>))};

    return std::ranges::any_of(
            mix_processor_typeids,
            equal_to(std::type_index(typeid(proc))));
}

#undef PIEJAM_MAX_NUM_FIXED_INPUTS_MIX_PROCESSOR

} // namespace piejam::audio::engine
