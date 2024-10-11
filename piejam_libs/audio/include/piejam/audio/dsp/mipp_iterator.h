// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/dsp/mipp.h>

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <bit>
#include <ranges>
#include <span>

namespace piejam::audio::dsp
{

namespace detail
{

template <mipp_number T>
struct mipp_iterator_output_proxy
{
    using value_type = std::remove_cv_t<T>;

    constexpr mipp_iterator_output_proxy() noexcept = default;
    constexpr mipp_iterator_output_proxy(T* p) noexcept
        : m_p{p}
    {
        BOOST_ASSERT(mipp::isAligned(m_p));
    }

    operator mipp::Reg<value_type>() const
    {
        return m_p;
    }

    auto
    operator=(mipp::Reg<value_type> const& r) & -> mipp_iterator_output_proxy&
        requires(!std::is_const_v<T>)
    {
        r.store(m_p);
        return *this;
    }

    auto operator=(mipp::Reg<value_type> const& r)
            const&& -> mipp_iterator_output_proxy
        requires(!std::is_const_v<T>)
    {
        r.store(m_p);
        return *this;
    }

    auto
    operator=(mipp::Regx2<value_type> const& r) & -> mipp_iterator_output_proxy&
        requires(!std::is_const_v<T>)
    {
        r[0].store(m_p);
        r[1].store(m_p + mipp::N<T>());
        return *this;
    }

    auto operator=(mipp::Regx2<value_type> const& r)
            const&& -> mipp_iterator_output_proxy
        requires(!std::is_const_v<T>)
    {
        r[0].store(m_p);
        r[1].store(m_p + mipp::N<T>());
        return *this;
    }

private:
    T* m_p{};
};

template <template <class> class Reg>
struct num_regs;

template <>
struct num_regs<mipp::Reg>
{
    static constexpr std::size_t value{1};
};

template <>
struct num_regs<mipp::Regx2>
{
    static constexpr std::size_t value{2};
};

template <template <class> class Reg>
constexpr std::size_t num_regs_v = num_regs<Reg>::value;

} // namespace detail

template <mipp_number T, template <class> class Reg = mipp::Reg>
struct mipp_iterator
    : boost::stl_interfaces::iterator_interface<
              mipp_iterator<T, Reg>,
              std::random_access_iterator_tag,
              Reg<std::remove_cv_t<T>>,
              std::conditional_t<
                      std::is_const_v<T>,
                      Reg<std::remove_cv_t<T>>,
                      detail::mipp_iterator_output_proxy<T>>,
              T*>
{
    static constexpr auto N =
            mipp::N<std::remove_cv_t<T>>() * detail::num_regs_v<Reg>;

    constexpr mipp_iterator() noexcept = default;
    constexpr mipp_iterator(T* p) noexcept
        : m_p(p)
    {
        BOOST_ASSERT(mipp::isAligned(m_p));
    }

    [[nodiscard]]
    constexpr auto operator*() const noexcept -> Reg<std::remove_cv_t<T>>
    {
        return m_p;
    }

    [[nodiscard]]
    constexpr auto operator*() noexcept -> detail::mipp_iterator_output_proxy<T>
        requires(!std::is_const_v<T>)
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
        BOOST_ASSERT((m_p - rhs.m_p) % N == 0);
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
make_mipp_iterator_x2(T* p)
{
    return mipp_iterator<T, mipp::Regx2>{p};
}

template <mipp_number T>
[[nodiscard]]
constexpr auto
make_mipp_iterator_x2(T const* p)
{
    return mipp_iterator<T const, mipp::Regx2>{p};
}

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_begin(std::span<T> const in)
{
    return mipp_iterator{in.data()};
}

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_end(std::span<T> const in)
{
    return mipp_iterator{in.data() + in.size()};
}

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_range(std::span<T> const in)
{
    auto const data = in.data();

    return std::ranges::subrange{
            mipp_iterator{data},
            mipp_iterator{data + in.size()}};
}

template <mipp_number T>
[[nodiscard]]
constexpr auto
mipp_range_from_unaligned(std::span<T> const in)
{
    auto const data = in.data();

    constexpr auto N = mipp::N<T>();

    auto start_offset = std::bit_cast<std::uintptr_t>(data) % N;
    auto rest_size = in.size() - std::min(in.size(), start_offset);
    auto post_offset = data + start_offset + (rest_size / N) * N;
    auto post_size = rest_size % N;

    return std::tuple{
            std::span{data, start_offset},
            std::ranges::subrange{
                    mipp_iterator{data + start_offset},
                    mipp_iterator{post_offset}},
            std::span{post_offset, post_size}};
}

} // namespace piejam::audio::dsp
