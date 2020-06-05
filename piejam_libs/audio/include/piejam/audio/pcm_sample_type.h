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

#pragma once

#include <piejam/audio/pcm_format.h>
#include <piejam/numeric/intops.h>
#include <piejam/numeric/intx.h>
#include <piejam/numeric/limits.h>
#include <piejam/numeric/type_traits.h>

#include <cstdint>
#include <type_traits>

namespace piejam::audio
{

template <pcm_format F, class T, bool LittleEndian>
struct pcm_sample_descriptor
{
    static constexpr pcm_format format = F;
    using value_type = T;
    static constexpr bool little_endian = LittleEndian;

    using signed_value_type = numeric::make_signed_t<value_type>;
    static constexpr value_type middle{
            numeric::intops::sign_map<value_type>(signed_value_type{})};
    static constexpr value_type min{numeric::limits<value_type>::min()};
    static constexpr value_type max{numeric::limits<value_type>::max()};
    static constexpr auto bitdepth{sizeof(value_type) * 8u};
    static_assert(bitdepth <= 32);
    using float_t = std::conditional_t<(bitdepth > 24), double, float>;
    static constexpr float_t fscale{1u << (bitdepth - 1)};
    static constexpr float_t fmiddle{};
    static constexpr float_t fmin{-1};
    static constexpr float_t fmax{
            numeric::intops::sign_map<signed_value_type>(max) / fscale};
};

template <pcm_format>
struct pcm_sample_type_map;

template <>
struct pcm_sample_type_map<pcm_format::s8>
{
    using type = pcm_sample_descriptor<pcm_format::s8, std::int8_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::u8>
{
    using type = pcm_sample_descriptor<pcm_format::u8, std::uint8_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::s16_le>
{
    using type = pcm_sample_descriptor<pcm_format::s16_le, std::int16_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::s16_be>
{
    using type = pcm_sample_descriptor<pcm_format::s16_be, std::int16_t, false>;
};

template <>
struct pcm_sample_type_map<pcm_format::u16_le>
{
    using type = pcm_sample_descriptor<pcm_format::u16_le, std::uint16_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::u16_be>
{
    using type =
            pcm_sample_descriptor<pcm_format::u16_be, std::uint16_t, false>;
};

template <>
struct pcm_sample_type_map<pcm_format::s32_le>
{
    using type = pcm_sample_descriptor<pcm_format::s32_le, std::int32_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::s32_be>
{
    using type = pcm_sample_descriptor<pcm_format::s32_be, std::int32_t, false>;
};

template <>
struct pcm_sample_type_map<pcm_format::u32_le>
{
    using type = pcm_sample_descriptor<pcm_format::u32_le, std::uint32_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::u32_be>
{
    using type =
            pcm_sample_descriptor<pcm_format::u32_be, std::uint32_t, false>;
};

template <>
struct pcm_sample_type_map<pcm_format::s24_3le>
{
    using type =
            pcm_sample_descriptor<pcm_format::s24_3le, numeric::int24_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::s24_3be>
{
    using type =
            pcm_sample_descriptor<pcm_format::s24_3be, numeric::int24_t, false>;
};

template <>
struct pcm_sample_type_map<pcm_format::u24_3le>
{
    using type =
            pcm_sample_descriptor<pcm_format::u24_3le, numeric::uint24_t, true>;
};

template <>
struct pcm_sample_type_map<pcm_format::u24_3be>
{
    using type = pcm_sample_descriptor<
            pcm_format::u24_3be,
            numeric::uint24_t,
            false>;
};

template <pcm_format F>
using pcm_sample_descriptor_t = typename pcm_sample_type_map<F>::type;

template <pcm_format F>
using pcm_sample_t = typename pcm_sample_type_map<F>::type::value_type;

} // namespace piejam::audio
