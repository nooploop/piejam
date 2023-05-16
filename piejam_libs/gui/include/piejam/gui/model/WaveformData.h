// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/shift_push_back.h>

#include <boost/assert.hpp>

#include <vector>

namespace piejam::gui::model
{

//! \invariant y0.size() == y1.size()
class WaveformData
{
public:
    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_data.empty();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return m_data.size() / 2;
    }

    [[nodiscard]] constexpr auto ys() const noexcept
            -> std::vector<float> const&
    {
        return m_data;
    }

    void clear()
    {
        m_data.clear();
    }

    void resize(std::size_t const sz)
    {
        m_data.resize(sz * 2, 0.f);
    }

    void reserve(std::size_t const capacity)
    {
        m_data.reserve(capacity * 2);
    }

    void push_back(float const y0, float const y1)
    {
        BOOST_ASSERT(-1.f <= y0 && y0 <= 1.f);
        BOOST_ASSERT(-1.f <= y1 && y1 <= 1.f);

        m_data.emplace_back(y0);
        m_data.emplace_back(y1);
    }

    void shift_push_back(WaveformData const& other)
    {
        algorithm::shift_push_back(m_data, other.m_data);
    }

    [[nodiscard]] auto operator==(WaveformData const&) const noexcept
            -> bool = default;

private:
    std::vector<float> m_data;
};

} // namespace piejam::gui::model
