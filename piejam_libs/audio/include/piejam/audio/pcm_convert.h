// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_sample_type.h>
#include <piejam/numeric/endian.h>
#include <piejam/numeric/intops.h>
#include <piejam/numeric/intx.h>

#include <algorithm>

namespace piejam::audio::pcm_convert
{

template <pcm_format F>
constexpr auto
endian_to_native(pcm_sample_t<F> x) noexcept -> pcm_sample_t<F>
{
    return pcm_sample_descriptor_t<F>::little_endian
                   ? numeric::endian::little_to_native(x)
                   : numeric::endian::big_to_native(x);
}

template <pcm_format F>
constexpr auto
endian_to_format(pcm_sample_t<F> x) noexcept -> pcm_sample_t<F>
{
    return pcm_sample_descriptor_t<F>::little_endian
                   ? numeric::endian::native_to_little(x)
                   : numeric::endian::native_to_big(x);
}

template <pcm_format F>
auto
from(pcm_sample_t<F> const x) noexcept -> float
{
    using desc_t = pcm_sample_descriptor_t<F>;
    using signed_t = typename desc_t::signed_value_type;
    return static_cast<float>(
            numeric::intops::sign_map<signed_t>(endian_to_native<F>(x)) /
            desc_t::fscale);
}

template <pcm_format F>
auto
to(float const x) noexcept -> pcm_sample_t<F>
{
    using desc_t = pcm_sample_descriptor_t<F>;
    using float_t = typename desc_t::float_t;
    using signed_t = typename desc_t::signed_value_type;
    return endian_to_format<F>(
            numeric::intops::sign_map<pcm_sample_t<F>>(static_cast<signed_t>(
                    std::clamp<float_t>(x, desc_t::fmin, desc_t::fmax) *
                    desc_t::fscale)));
}

} // namespace piejam::audio::pcm_convert
