// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::runtime
{

namespace detail
{

template <class P>
struct parameter_maps_access;

} // namespace detail

class parameter_maps
{
public:
    parameter_maps();
    parameter_maps(parameter_maps const&);

    ~parameter_maps();

    auto operator=(parameter_maps const&) -> parameter_maps&;

private:
    template <class P>
    friend struct detail::parameter_maps_access;

    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace piejam::runtime
