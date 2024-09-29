// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/dsp/mipp.h>

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <ranges>
#include <span>

namespace piejam::audio::dsp
{

template <mipp_number T>
struct mipp_iterator
    : boost::stl_interfaces::iterator_interface<
              mipp_iterator<T>,
              std::random_access_iterator_tag,
              mipp::Reg<std::remove_const_t<T>>,
              mipp::Reg<std::remove_const_t<T>>,
              T*>
{
    static constexpr auto N = mipp::N<std::remove_const_t<T>>();

    constexpr mipp_iterator() noexcept = default;
    constexpr mipp_iterator(T* p) noexcept
        : m_p(p)
    {
        BOOST_ASSERT(mipp::isAligned(m_p));
    }

    [[nodiscard]]
    constexpr auto
    operator*() const noexcept -> mipp::Reg<std::remove_const_t<T>>
    {
        return m_p;
    }

    constexpr auto operator+=(std::ptrdiff_t n) noexcept -> mipp_iterator&
    {
        m_p += n * N;
        return *this;
    }

    [[nodiscard]]
    constexpr auto
    operator-(mipp_iterator const& rhs) const noexcept -> std::ptrdiff_t
    {
        return (m_p - rhs.m_p) / N;
    }

    [[nodiscard]]
    constexpr auto operator==(mipp_iterator const& rhs) const noexcept -> bool
    {
        return m_p == rhs.m_p;
    }

    [[nodiscard]]
    constexpr auto operator<(mipp_iterator const& rhs) const noexcept -> bool
    {
        return m_p < rhs.m_p;
    }

private:
    T* m_p{};
};

template <mipp_number T>
mipp_iterator(T*) -> mipp_iterator<T>;

template <mipp_number T>
mipp_iterator(T const*) -> mipp_iterator<T const>;

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_range(std::span<T> const in)
{
    auto const data = in.data();
    auto const size = in.size();

    BOOST_ASSERT(mipp::isAligned(data));
    BOOST_ASSERT(size % mipp::N<T>() == 0);

    return std::ranges::subrange{
            mipp_iterator{data},
            mipp_iterator{data + size}};
}

} // namespace piejam::audio::dsp
