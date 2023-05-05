// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/strided_span.h>

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <algorithm>

namespace piejam::range
{

template <
        class T,
        std::size_t MajorSize = std::dynamic_extent,
        std::size_t MinorSize = std::dynamic_extent,
        std::ptrdiff_t MajorStep = dynamic_stride,
        std::ptrdiff_t MinorStep = dynamic_stride>
class table_view
{
public:
    template <class U>
    class major_index_iterator
        : public boost::stl_interfaces::iterator_interface<
                  major_index_iterator<U>,
                  std::random_access_iterator_tag,
                  strided_span<U, MinorStep>,
                  strided_span<U, MinorStep> const&,
                  strided_span<U, MinorStep> const*>
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = strided_span<U, MinorStep>;
        using difference_type = std::ptrdiff_t;
        using pointer = strided_span<U, MinorStep> const*;
        using reference = strided_span<U, MinorStep> const&;

        constexpr major_index_iterator() noexcept = default;

        constexpr major_index_iterator(
                U* const data,
                difference_type const step,
                std::size_t const minor_size,
                difference_type const minor_step) noexcept
            requires(MinorStep == dynamic_stride)
            : m_stride{data, minor_size, minor_step}
            , m_step{step}
        {
            BOOST_ASSERT(
                    (MajorStep == dynamic_stride && step != 0) ||
                    MajorStep == step);
            BOOST_ASSERT(
                    MinorSize == std::dynamic_extent ||
                    MinorSize == minor_size);
            BOOST_ASSERT(
                    (MinorStep == dynamic_stride && minor_step != 0) ||
                    MinorStep == minor_step);
        }

        constexpr major_index_iterator(
                U* const data,
                difference_type const step,
                std::size_t const minor_size,
                difference_type const minor_step) noexcept
            requires(MinorStep != dynamic_stride)
            : m_stride{data, minor_size}
            , m_step{step}
        {
            BOOST_ASSERT(
                    (MajorStep == dynamic_stride && step != 0) ||
                    MajorStep == step);
            BOOST_ASSERT(
                    MinorSize == std::dynamic_extent ||
                    MinorSize == minor_size);
            BOOST_ASSERT(MinorStep == minor_step);
        }

        [[nodiscard]] constexpr auto
        operator==(major_index_iterator const& other) const noexcept -> bool
        {
            return m_stride.data() == other.m_stride.data() &&
                   m_stride.size() == other.m_stride.size() &&
                   m_stride.stride() == other.m_stride.stride() &&
                   m_step == other.m_step;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> reference
        {
            return m_stride;
        }

        [[nodiscard]] constexpr auto operator+=(std::ptrdiff_t n) noexcept
                -> major_index_iterator&
        {
            if constexpr (MinorStep == dynamic_stride)
            {
                m_stride = {
                        m_stride.data() + n * step(),
                        m_stride.size(),
                        m_stride.stride()};
            }
            else
            {
                m_stride = {m_stride.data() + n * step(), m_stride.size()};
            }
            return *this;
        }

        [[nodiscard]] constexpr auto
        operator-(major_index_iterator const& other) const noexcept
        {
            BOOST_ASSERT(m_stride.size() == other.m_stride.size());
            BOOST_ASSERT(m_stride.stride() == other.m_stride.stride());
            BOOST_ASSERT(step() == other.step());
            return (m_stride.data() - other.m_stride.data()) / step();
        }

    private:
        [[nodiscard]] constexpr auto step() const noexcept -> difference_type
        {
            return MajorStep == dynamic_stride ? m_step : MajorStep;
        }

        strided_span<U, MinorStep> m_stride;
        difference_type m_step{MajorStep == dynamic_stride ? 1 : MajorStep};
    };

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = major_index_iterator<T>;
    using const_iterator = major_index_iterator<T const>;

    constexpr table_view() noexcept = default;

    constexpr table_view(
            T* const data,
            size_type const major_size,
            size_type const minor_size,
            difference_type const major_step,
            difference_type const minor_step) noexcept
        : m_data{data}
        , m_major_size{major_size}
        , m_minor_size{minor_size}
        , m_major_step{major_step}
        , m_minor_step{minor_step}
    {
        BOOST_ASSERT(
                MajorSize == std::dynamic_extent || MajorSize == major_size);
        BOOST_ASSERT(
                MinorSize == std::dynamic_extent || MinorSize == minor_size);
        BOOST_ASSERT(
                (MajorStep == dynamic_stride && major_step != 0) ||
                MajorStep == major_step);
        BOOST_ASSERT(
                (MinorStep == dynamic_stride && minor_step != 0) ||
                MinorStep == minor_step);
    }

    [[nodiscard]] constexpr auto data() const noexcept -> T*
    {
        return m_data;
    }

    [[nodiscard]] constexpr auto major_size() const noexcept -> size_type
    {
        return MajorSize == std::dynamic_extent ? m_major_size : MajorSize;
    }

    [[nodiscard]] constexpr auto minor_size() const noexcept -> size_type
    {
        return MinorSize == std::dynamic_extent ? m_minor_size : MinorSize;
    }

    [[nodiscard]] constexpr auto major_step() const noexcept -> difference_type
    {
        return MajorStep == dynamic_stride ? m_major_step : MajorStep;
    }

    [[nodiscard]] constexpr auto minor_step() const noexcept -> difference_type
    {
        return MinorStep == dynamic_stride ? m_minor_step : MinorStep;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator
    {
        return {m_data, major_step(), minor_size(), minor_step()};
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return {m_data, major_step(), minor_size(), minor_step()};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator
    {
        return {m_data + major_size() * major_step(),
                major_step(),
                minor_size(),
                minor_step()};
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
    {
        return {m_data + major_size() * major_step(),
                major_step(),
                minor_size(),
                minor_step()};
    }

    [[nodiscard]] constexpr auto
    operator[](size_type major_index) const noexcept
            -> strided_span<T, MinorStep>
    {
        if constexpr (MinorStep == dynamic_stride)
        {
            return {m_data + major_index * major_step(),
                    minor_size(),
                    minor_step()};
        }
        else
        {
            return {m_data + major_index * major_step(), minor_size()};
        }
    }

    template <
            std::size_t ToMajorSize,
            std::size_t ToMinorSize,
            std::ptrdiff_t ToMajorStep,
            std::ptrdiff_t ToMinorStep>
    [[nodiscard]] constexpr auto cast() const noexcept
            -> table_view<T, ToMajorSize, ToMinorSize, ToMajorStep, ToMinorStep>
    {
        static_assert(
                MajorSize == std::dynamic_extent ||
                ToMajorSize == std::dynamic_extent || MajorSize == ToMajorSize);
        static_assert(
                MinorSize == std::dynamic_extent ||
                ToMinorSize == std::dynamic_extent || MinorSize == ToMinorSize);
        static_assert(
                MajorStep == dynamic_stride || ToMajorSize == dynamic_stride ||
                MajorStep == ToMajorStep);
        static_assert(
                MinorStep == dynamic_stride || ToMinorStep == dynamic_stride ||
                MinorStep == ToMinorStep);

        if constexpr (ToMajorSize != std::dynamic_extent)
        {
            BOOST_ASSERT(major_size() == ToMajorSize);
        }

        if constexpr (ToMinorSize != std::dynamic_extent)
        {
            BOOST_ASSERT(minor_size() == ToMinorSize);
        }

        if constexpr (ToMajorStep != dynamic_stride)
        {
            BOOST_ASSERT(major_step() == ToMajorStep);
        }

        if constexpr (ToMinorStep != dynamic_stride)
        {
            BOOST_ASSERT(minor_step() == ToMinorStep);
        }

        return table_view<
                T,
                ToMajorSize,
                ToMinorSize,
                ToMajorStep,
                ToMinorStep>{
                m_data,
                major_size(),
                minor_size(),
                major_step(),
                minor_step()};
    }

private:
    T* m_data{};
    size_type m_major_size{};
    size_type m_minor_size{};
    difference_type m_major_step{MajorStep == dynamic_stride ? 1 : MajorStep};
    difference_type m_minor_step{MinorStep == dynamic_stride ? 1 : MinorStep};
};

template <class T>
table_view(
        T*,
        typename table_view<T>::size_type,
        typename table_view<T>::size_type,
        typename table_view<T>::difference_type,
        typename table_view<T>::difference_type) -> table_view<T>;

template <
        class T,
        std::size_t MajorSize,
        std::size_t MinorSize,
        std::ptrdiff_t MajorStep,
        std::ptrdiff_t MinorStep>
auto
transpose(table_view<T, MajorSize, MinorSize, MajorStep, MinorStep> const&
                  t) noexcept
        -> table_view<T, MinorSize, MajorSize, MinorStep, MajorStep>
{
    return {t.data(),
            t.minor_size(),
            t.major_size(),
            t.minor_step(),
            t.major_step()};
}

template <
        class T,
        std::size_t TMajorSize,
        std::size_t TMinorSize,
        std::ptrdiff_t TMajorStep,
        std::ptrdiff_t TMinorStep,
        class U,
        std::size_t UMajorSize,
        std::size_t UMinorSize,
        std::ptrdiff_t UMajorStep,
        std::ptrdiff_t UMinorStep,
        class UnaryOperation>
auto
transform(
        table_view<T, TMajorSize, TMinorSize, TMajorStep, TMinorStep> const&
                src,
        table_view<U, UMajorSize, UMinorSize, UMajorStep, UMinorStep> const&
                dst,
        UnaryOperation&& op)
{
    static_assert(
            TMajorSize == std::dynamic_extent ||
            UMajorSize == std::dynamic_extent || TMajorSize == UMajorSize);
    static_assert(
            TMinorSize == std::dynamic_extent ||
            UMinorSize == std::dynamic_extent || TMinorSize == UMinorSize);

    BOOST_ASSERT(src.major_size() == dst.major_size());
    BOOST_ASSERT(src.minor_size() == dst.minor_size());

    for (auto src_it = src.begin(), src_end = src.end(), dst_it = dst.begin();
         src_it != src_end;
         ++src_it, ++dst_it)
    {
        std::ranges::transform(*src_it, dst_it->begin(), op);
    }
}

template <
        class T,
        std::size_t MajorSize,
        std::size_t MinorSize,
        std::ptrdiff_t MajorStep,
        std::ptrdiff_t MinorStep>
void
fill(table_view<T, MajorSize, MinorSize, MajorStep, MinorStep> const& tv,
     T const value)
{
    for (auto row : tv)
    {
        std::ranges::fill(row, value);
    }
}

template <
        class T,
        std::size_t MajorSize,
        std::size_t MinorSize,
        std::ptrdiff_t MajorStep,
        std::ptrdiff_t MinorStep>
auto
as_const(table_view<T, MajorSize, MinorSize, MajorStep, MinorStep> const& tv)
        -> table_view<
                std::add_const_t<T>,
                MajorSize,
                MinorSize,
                MajorStep,
                MinorStep>
{
    return {tv.data(),
            tv.major_size(),
            tv.minor_size(),
            tv.major_step(),
            tv.minor_step()};
}

} // namespace piejam::range
