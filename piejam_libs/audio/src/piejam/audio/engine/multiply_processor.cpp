// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/multiply_processor.h>

#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/slice_algorithms.h>

#include <piejam/npos.h>

#include <boost/assert.hpp>
#include <boost/preprocessor/iteration/local.hpp>

namespace piejam::audio::engine
{

namespace
{

template <std::size_t N>
auto
multiply(process_context const& ctx)
{
    return multiply(
            ctx.inputs[N - 1].get(),
            multiply<N - 1>(ctx),
            ctx.outputs[0]);
}

template <>
auto
multiply<2>(process_context const& ctx)
{
    return multiply(ctx.inputs[1].get(), ctx.inputs[0].get(), ctx.outputs[0]);
}

template <>
auto
multiply<npos>(process_context const& ctx)
{
    slice<float> res{1.f};

    auto const out = ctx.outputs[0];

    for (slice<float> const& in : ctx.inputs)
    {
        res = multiply(in, res, out);
    }

    return res;
}

template <std::size_t NumInputs>
class multiply_processor final : public named_processor
{
public:
    multiply_processor(std::string_view const name)
        requires(NumInputs != npos)
        : named_processor(name)
        , m_num_inputs(NumInputs)
    {
    }

    multiply_processor(std::size_t num_inputs, std::string_view const name)
        requires(NumInputs == npos)
        : named_processor(name)
        , m_num_inputs(num_inputs)
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "multiply";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return NumInputs != npos ? NumInputs : m_num_inputs;
    }

    auto num_outputs() const noexcept -> std::size_t override
    {
        return 1;
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        return {};
    }
    auto event_outputs() const noexcept -> event_ports override
    {
        return {};
    }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);

        ctx.results[0] = multiply<NumInputs>(ctx);
    }

private:
    std::size_t const m_num_inputs{};
};

} // namespace

#define PIEJAM_MAX_NUM_FIXED_INPUTS_MULTIPLY_PROCESSOR 8

auto
make_multiply_processor(
        std::size_t const num_inputs,
        std::string_view const name) -> std::unique_ptr<processor>
{
    switch (num_inputs)
    {

#define BOOST_PP_LOCAL_LIMITS                                                  \
    (2, PIEJAM_MAX_NUM_FIXED_INPUTS_MULTIPLY_PROCESSOR)
#define BOOST_PP_LOCAL_MACRO(n)                                                \
    case n:                                                                    \
        return std::make_unique<multiply_processor<n>>(name);
#include BOOST_PP_LOCAL_ITERATE()

        default:
            BOOST_ASSERT(num_inputs > 1);
            return std::make_unique<multiply_processor<npos>>(num_inputs, name);
    }
}

#undef PIEJAM_MAX_NUM_FIXED_INPUTS_MULTIPLY_PROCESSOR

} // namespace piejam::audio::engine
