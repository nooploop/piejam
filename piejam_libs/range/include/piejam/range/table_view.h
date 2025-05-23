// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/strided_span.h>

#include <boost/assert.hpp>

#include <span>

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
    template <class U>
    using minor_span_type = std::conditional_t<
            MinorStep == 1,
            std::span<T>,
            strided_span<U, MinorStep>>;

public:
    template <class U>
    class major_index_iterator
    {

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = minor_span_type<U>;
        using reference = value_type;
        using pointer = value_type const*;
        using difference_type = std::ptrdiff_t;

        constexpr major_index_iterator() noexcept = default;

        constexpr major_index_iterator(
                U* const data,
                difference_type const step,
                std::size_t const minor_size,
                difference_type const minor_step) noexcept
            requires(MinorStep == dynamic_stride)
            : m_minor_span{data, minor_size, minor_step}
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
                [[maybe_unused]] difference_type const minor_step) noexcept
            requires(MinorStep != dynamic_stride)
            : m_minor_span{data, minor_size}
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

        [[nodiscard]]
        constexpr auto operator*() const noexcept -> reference
        {
            return m_minor_span;
        }

        [[nodiscard]]
        constexpr auto operator->() const noexcept -> pointer
        {
            return &m_minor_span;
        }

        [[nodiscard]]
        constexpr auto operator[](difference_type n) const noexcept -> reference
        {
            return *(*this + n);
        }

        constexpr auto
        operator+=(std::ptrdiff_t n) noexcept -> major_index_iterator&
        {
            if constexpr (MinorStep == dynamic_stride)
            {
                m_minor_span = {
                        m_minor_span.data() + n * step(),
                        m_minor_span.size(),
                        m_minor_span.stride()};
            }
            else
            {
                m_minor_span = {
                        m_minor_span.data() + n * step(),
                        m_minor_span.size()};
            }
            return *this;
        }

        constexpr auto operator++() noexcept -> major_index_iterator&
        {
            *this += 1;
            return *this;
        }

        [[nodiscard]]
        constexpr auto operator++(int) noexcept -> major_index_iterator
        {
            major_index_iterator temp(*this);
            ++(*this);
            return temp;
        }

        constexpr auto operator--() noexcept -> major_index_iterator&
        {
            *this += -1;
            return *this;
        }

        [[nodiscard]]
        constexpr auto operator--(int) noexcept -> major_index_iterator
        {
            major_index_iterator temp(*this);
            --(*this);
            return temp;
        }

        [[nodiscard]]
        constexpr auto
        operator+(difference_type n) const noexcept -> major_index_iterator
        {
            major_index_iterator temp(*this);
            return temp += n;
        }

        [[nodiscard]]
        friend constexpr auto
        operator+(difference_type n, major_index_iterator const& it) noexcept
                -> major_index_iterator
        {
            return it + n;
        }

        constexpr auto
        operator-=(difference_type n) noexcept -> major_index_iterator&
        {
            return *this += -n;
        }

        [[nodiscard]]
        constexpr auto
        operator-(difference_type n) const noexcept -> major_index_iterator
        {
            major_index_iterator temp(*this);
            return temp -= n;
        }

        [[nodiscard]]
        constexpr auto operator-(major_index_iterator const& other)
                const noexcept -> difference_type
        {
            verify_precondition(other);
            return (m_minor_span.data() - other.m_minor_span.data()) / step();
        }

        [[nodiscard]]
        constexpr auto
        operator==(major_index_iterator const& rhs) const noexcept -> bool
        {
            verify_precondition(rhs);
            return m_minor_span.data() == rhs.m_minor_span.data();
        }

        [[nodiscard]]
        constexpr auto
        operator!=(major_index_iterator const& rhs) const noexcept -> bool
        {
            return !(*this == rhs);
        }

        [[nodiscard]]
        constexpr auto
        operator<(major_index_iterator const& rhs) const noexcept -> bool
        {
            verify_precondition(rhs);
            return m_minor_span.data() < rhs.m_minor_span.data();
        }

        [[nodiscard]]
        constexpr auto
        operator>(major_index_iterator const& rhs) const noexcept -> bool
        {
            return rhs < *this;
        }

        [[nodiscard]]
        constexpr auto
        operator<=(major_index_iterator const& rhs) const noexcept -> bool
        {
            return !(rhs < *this);
        }

        [[nodiscard]]
        constexpr auto
        operator>=(major_index_iterator const& rhs) const noexcept -> bool
        {
            return !(*this < rhs);
        }

    private:
        constexpr auto
        verify_precondition(major_index_iterator const& rhs) const noexcept
        {
            BOOST_ASSERT(step() == rhs.step());
            BOOST_ASSERT(m_minor_span.size() == rhs.m_minor_span.size());
            if constexpr (MinorStep != 1)
            {
                BOOST_ASSERT(
                        m_minor_span.stride() == rhs.m_minor_span.stride());
                BOOST_ASSERT(
                        (m_minor_span.data() - rhs.m_minor_span.data()) %
                                step() ==
                        0);
            }
            (void)rhs;
        }

        [[nodiscard]]
        constexpr auto step() const noexcept -> difference_type
        {
            return MajorStep == dynamic_stride ? m_step : MajorStep;
        }

        value_type m_minor_span;
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

    [[nodiscard]]
    constexpr auto data() const noexcept -> T*
    {
        return m_data;
    }

    [[nodiscard]]
    constexpr auto major_size() const noexcept -> size_type
    {
        return MajorSize == std::dynamic_extent ? m_major_size : MajorSize;
    }

    [[nodiscard]]
    constexpr auto minor_size() const noexcept -> size_type
    {
        return MinorSize == std::dynamic_extent ? m_minor_size : MinorSize;
    }

    [[nodiscard]]
    constexpr auto major_step() const noexcept -> difference_type
    {
        return MajorStep == dynamic_stride ? m_major_step : MajorStep;
    }

    [[nodiscard]]
    constexpr auto minor_step() const noexcept -> difference_type
    {
        return MinorStep == dynamic_stride ? m_minor_step : MinorStep;
    }

    [[nodiscard]]
    constexpr auto begin() const noexcept -> iterator
    {
        return {m_data, major_step(), minor_size(), minor_step()};
    }

    [[nodiscard]]
    constexpr auto cbegin() const noexcept -> const_iterator
    {
        return {m_data, major_step(), minor_size(), minor_step()};
    }

    [[nodiscard]]
    constexpr auto end() const noexcept -> iterator
    {
        return {m_data + major_size() * major_step(),
                major_step(),
                minor_size(),
                minor_step()};
    }

    [[nodiscard]]
    constexpr auto cend() const noexcept -> const_iterator
    {
        return {m_data + major_size() * major_step(),
                major_step(),
                minor_size(),
                minor_step()};
    }

    [[nodiscard]]
    constexpr auto
    operator[](size_type major_index) const noexcept -> minor_span_type<T>
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

} // namespace piejam::range
