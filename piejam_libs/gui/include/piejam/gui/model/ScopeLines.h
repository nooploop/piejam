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
class ScopeLines
{
public:
    bool empty() const noexcept { return m_y0.empty(); }
    auto size() const noexcept -> std::size_t { return m_y0.size(); }

    constexpr auto y0() const noexcept -> std::vector<float> const&
    {
        return m_y0;
    }

    constexpr auto y1() const noexcept -> std::vector<float> const&
    {
        return m_y1;
    }

    void clear()
    {
        m_y0.clear();
        m_y1.clear();
    }

    void resize(std::size_t const sz)
    {
        m_y0.resize(sz);
        m_y1.resize(sz);
    }

    void reserve(std::size_t const capacity)
    {
        m_y0.reserve(capacity);
        m_y1.reserve(capacity);
    }

    void push_back(float const y0, float const y1)
    {
        BOOST_ASSERT(-1.f <= y0 && y0 <= 1.f);
        BOOST_ASSERT(-1.f <= y1 && y1 <= 1.f);

        m_y0.emplace_back(y0);
        m_y1.emplace_back(y1);
    }

    void shift_push_back(ScopeLines const& other)
    {
        algorithm::shift_push_back(m_y0, other.m_y0);
        algorithm::shift_push_back(m_y1, other.m_y1);
    }

    bool operator==(ScopeLines const&) const noexcept = default;

private:
    std::vector<float> m_y0;
    std::vector<float> m_y1;
};

} // namespace piejam::gui::model
