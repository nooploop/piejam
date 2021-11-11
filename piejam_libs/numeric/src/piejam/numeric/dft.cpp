// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/numeric/dft.h>

#include <fftw3.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <vector>

namespace piejam::numeric
{

namespace
{

template <class T>
struct fftwf_allocator
{
    using value_type = T;
    using is_always_equal = std::true_type;

    auto allocate(std::size_t const n) -> T*
    {
        return static_cast<T*>(fftwf_malloc(sizeof(T) * n));
    }

    void deallocate(T* p, std::size_t) { fftwf_free(p); }
};

struct fftwf_plan_deleter
{
    void operator()(fftwf_plan p) { fftwf_destroy_plan(p); }
};

} // namespace

struct dft::impl
{
    static_assert(sizeof(std::complex<float>) == sizeof(fftwf_complex));

    template <class T>
    using fftwf_vector = std::vector<T, fftwf_allocator<T>>;
    using fftwf_real_vector = fftwf_vector<float>;
    using fftwf_complex_vector = fftwf_vector<std::complex<float>>;
    using fftwf_plan_unique_ptr = std::
            unique_ptr<std::remove_pointer_t<fftwf_plan>, fftwf_plan_deleter>;

    std::size_t input_size{};
    std::size_t output_size{input_size / 2 + 1};

    fftwf_real_vector in_buffer{fftwf_real_vector(input_size)};
    fftwf_complex_vector out_buffer{fftwf_complex_vector(output_size)};
    fftwf_plan_unique_ptr plan{fftwf_plan_dft_r2c_1d(
            input_size,
            in_buffer.data(),
            reinterpret_cast<fftwf_complex*>(out_buffer.data()),
            FFTW_MEASURE)};
};

dft::dft(std::size_t const size)
    : m_impl(std::make_unique<impl>(size))
{
}

dft::~dft() = default;

auto
dft::input_size() const noexcept -> std::size_t
{
    return m_impl->input_size;
}

auto
dft::output_size() const noexcept -> std::size_t
{
    return m_impl->output_size;
}

auto
dft::process(std::span<const float> const in)
        -> std::span<const std::complex<float>>
{
    BOOST_ASSERT(in.size() == m_impl->input_size);

    std::ranges::copy(in, m_impl->in_buffer.begin());

    fftwf_execute(m_impl->plan.get());

    return m_impl->out_buffer;
}

} // namespace piejam::numeric
